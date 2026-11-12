#!/usr/bin/env python3
"""
Adds five example dash pages to the EEZ project, for comparing layouts and
their render cost on the board (see the lv_port perf line on the console):

  ex_tacho   round tacho with tick scale, short pointer on the rim,
             speed in the centre, value columns left/right
  ex_needle  same scale with a full needle line from the centre,
             value columns with separator lines
  ex_bars    nine horizontal bars: name | bar | value | unit
  ex_gauges  six small arc gauges with tick scales (2 x 3)
  ex_strip   RPM strip + shift lights, big RPM / speed, six values with bars

Run it again at any time: the ex_* pages are replaced, nothing else changes.
Missing variables are added (numeric *_val variables feed bars and gauges,
mapped in components/canmap/default_map.xml).

    python3 tools/eez_example_pages.py && tools/eez-build.sh

Widgets are built from the JSON of widgets already in the project (same keys
EEZ Studio writes), with fresh objIDs. Edit the pages afterwards in EEZ
Studio, or change this script and run it again.
"""
import copy
import json
import os
import uuid

HERE = os.path.dirname(os.path.abspath(__file__))
PROJECT = os.path.join(HERE, '..', 'components', 'hmi', 'FP_Dash.eez-project')
PAGES = ['ex_tacho', 'ex_needle', 'ex_bars', 'ex_gauges', 'ex_strip']

# variable name -> (type, default)
NEW_VARS = {
    'coolant_temp_val': ('integer', '0'),
    'intake_temp_val': ('integer', '0'),
    'oil_temperature_val': ('integer', '0'),
    'oil_press_val': ('integer', '0'),
    'map_val': ('integer', '0'),
    'batt_volt_x10': ('integer', '0'),
    'tps1_val': ('integer', '0'),
    'lam1_x100': ('integer', '0'),
    'fp_low_val': ('integer', '0'),
    'fuel_level_val': ('integer', '0'),
    'fuel_level_text': ('string', '"0"'),
}

WHITE, GREY, DARK = '#ffffff', '#9a9a9a', '#2a2a2a'
ORANGE, RED, YELLOW = '#ff7a00', '#ff3030', '#ffd040'


def uid():
    return str(uuid.uuid4())


def fresh_ids(o):
    """New objID everywhere (widgets, styles)."""
    if isinstance(o, dict):
        if 'objID' in o:
            o['objID'] = uid()
        for v in o.values():
            fresh_ids(v)
    elif isinstance(o, list):
        for v in o:
            fresh_ids(v)
    return o


class Builder:
    def __init__(self, project):
        self.tpl = {}
        for pg in project['userPages']:
            for w in self._walk(pg['components'][0]):
                self.tpl.setdefault(w['type'], w)
        self.screen_tpl = project['userPages'][0]

    def _walk(self, w):
        yield w
        for c in w.get('children', []) or []:
            yield from self._walk(c)

    def base(self, wtype, x, y, w, h, styles, tpl_type=None):
        t = copy.deepcopy(self.tpl[tpl_type or wtype])
        t['type'] = wtype
        t.update(left=x, top=y, width=w, height=h, leftUnit='px', topUnit='px',
                 widthUnit='px', heightUnit='px', children=[], eventHandlers=[],
                 hiddenFlagType='literal', group='', groupIndex=0)
        t.pop('hiddenFlag', None)
        t.pop('identifier', None)
        t['localStyles'] = {'objID': uid(), 'definition': styles}
        t['style'] = {'objID': uid(), 'useStyle': 'default', 'conditionalStyles': [], 'childStyles': []}
        return fresh_ids(t)

    def label(self, text, x, y, w=None, font=20, color=WHITE, align=None, expr=False, hidden=None):
        st = {'text_color': color, 'text_font': f'MONTSERRAT_{font}'}
        if align:
            st['text_align'] = align
        t = self.base('LVGLLabelWidget', x, y, w or 0, 30, {'MAIN': {'DEFAULT': st}})
        t['widthUnit'] = 'px' if w else 'content'
        t['heightUnit'] = 'content'
        t.update(text=text, textType='expression' if expr else 'literal', longMode='CLIP', recolor=False)
        if hidden:
            t.update(hiddenFlagType='expression', hiddenFlag=hidden)
        return t

    def value(self, var, x, y, w, font=30, color=WHITE, align='CENTER'):
        return self.label(var, x, y, w, font, color, align, expr=True)

    def panel(self, x, y, w, h, bg='#141414', border=None, radius=0, children=(), hidden=None, opa=255):
        st = {'pad_top': 0, 'pad_bottom': 0, 'pad_left': 0, 'pad_right': 0,
              'bg_color': bg, 'bg_opa': opa, 'radius': radius,
              'border_width': border[0] if border else 0}
        if border:
            st['border_color'] = border[1]
        t = self.base('LVGLPanelWidget', x, y, w, h, {'MAIN': {'DEFAULT': st}})
        t['children'] = list(children)
        if hidden:
            t.update(hiddenFlagType='expression', hiddenFlag=hidden)
        return t

    def bar(self, var, vmin, vmax, x, y, w, h, color, bg=DARK, radius=0):
        t = self.base('LVGLBarWidget', x, y, w, h, {
            'MAIN': {'DEFAULT': {'bg_color': bg, 'radius': radius}},
            'INDICATOR': {'DEFAULT': {'bg_color': color, 'radius': radius}}})
        t.update(min=vmin, minType='literal', max=vmax, maxType='literal', mode='NORMAL',
                 value=var, valueType='expression', previewValue=(vmin + vmax) // 2,
                 valueLeft=0, valueLeftType='literal', previewValueLeft=0, enableAnimation=False)
        return t

    def arc(self, var, vmin, vmax, x, y, size, width, color, bg=DARK, start=150, end=30):
        pad = {'pad_top': 0, 'pad_bottom': 0, 'pad_left': 0, 'pad_right': 0}
        t = self.base('LVGLArcWidget', x, y, size, size, {
            'MAIN': {'DEFAULT': dict(arc_color=bg, arc_width=width, arc_rounded=False, **pad)},
            'INDICATOR': {'DEFAULT': {'arc_color': color, 'arc_width': width, 'arc_rounded': False}},
            'KNOB': {'DEFAULT': dict(bg_opa=0, **pad)}})
        t.update(rangeMin=vmin, rangeMinType='literal', rangeMax=vmax, rangeMaxType='literal',
                 value=var, valueType='expression', bgStartAngle=start, bgEndAngle=end,
                 mode='NORMAL', rotation=0)
        return t

    def meter(self, x, y, size, scales, tick_font=24, tick_color=WHITE):
        """scales: list of dicts in EEZ LVGLMeterScale form (see scale())."""
        t = self.base('LVGLMeterWidget', x, y, size, size, {
            'MAIN': {'DEFAULT': {'bg_opa': 0, 'border_width': 0, 'pad_top': 0, 'pad_bottom': 0,
                                 'pad_left': 0, 'pad_right': 0}},
            'TICKS': {'DEFAULT': {'text_color': tick_color, 'text_font': f'MONTSERRAT_{tick_font}'}}},
            tpl_type='LVGLArcWidget')
        for k in ('rangeMin', 'rangeMinType', 'rangeMax', 'rangeMaxType', 'value', 'valueType',
                  'bgStartAngle', 'bgEndAngle', 'mode', 'rotation'):
            t.pop(k, None)
        t['widgetFlags'] = 'GESTURE_BUBBLE|SNAPPABLE'
        t['clickableFlag'] = False
        t['scales'] = scales
        return t


def scale(vmin, vmax, ticks, tick_w, tick_len, tick_color, angle=270, rotation=135,
          major_every=0, major_w=0, major_len=0, major_color=WHITE, label_gap=10, indicators=()):
    return {'objID': uid(), 'scaleMin': vmin, 'scaleMax': vmax, 'scaleAngleRange': angle,
            'scaleRotation': rotation, 'minorTickCount': ticks, 'minorTickLineWidth': tick_w,
            'minorTickLength': tick_len, 'minorTickColor': tick_color, 'nthMajor': major_every,
            'majorTickWidth': major_w, 'majorTickLength': major_len, 'majorTickColor': major_color,
            'labelGap': label_gap, 'indicators': list(indicators)}


def ind_needle(var, width, color, radius_mod=0):
    return {'objID': uid(), 'type': 'NEEDLE_LINE', 'width': width, 'color': color,
            'radiusModifier': radius_mod, 'value': var, 'valueType': 'expression'}


def ind_arc(start, end, width, color, radius_mod=0, start_expr=True, end_expr=True):
    return {'objID': uid(), 'type': 'ARC', 'width': width, 'color': color, 'radiusModifier': radius_mod,
            'startValue': start, 'startValueType': 'expression' if start_expr else 'literal',
            'endValue': end, 'endValueType': 'expression' if end_expr else 'literal'}


def ind_scale_lines(start, end, color_start, color_end):
    return {'objID': uid(), 'type': 'SCALE_LINES', 'colorStart': color_start, 'colorEnd': color_end,
            'local': False, 'widthModifier': 0, 'startValue': start, 'startValueType': 'literal',
            'endValue': end, 'endValueType': 'literal'}


def tacho_scales(indicators, tick_font_gap=14):
    """0..8 x1000 rpm, 270 deg: labelled major ticks + fine rpm ticks with a red zone."""
    return [
        scale(0, 8, 9, 4, 22, WHITE, major_every=1, major_w=4, major_len=22, label_gap=tick_font_gap),
        scale(0, 8000, 41, 2, 10, '#b0b0b0',
              indicators=[ind_scale_lines(6500, 8000, RED, RED)] + list(indicators)),
    ]


# ---------------------------------------------------------------- pages

def page_tacho(b):
    kids = []
    left = [('WATER', 'coolant_temp_text'), ('OIL P', 'oil_press_text'),
            ('OIL T', 'oil_temperature_text'), ('FUEL P', 'fp_low_text')]
    right = [('BATT V', 'batt_volt_text'), ('IAT', 'intake_temp_text'),
             ('MAP', 'map_text'), ('FUEL L', 'fuel_level_text')]
    for col, x in ((left, 20), (right, 630)):
        for i, (name, var) in enumerate(col):
            y = 40 + i * 108
            kids.append(b.label(name, x, y, 150, 22, GREY, 'CENTER'))
            kids.append(b.value(var, x + 15, y + 32, 120, 40))
    # short pointer on the rim: an arc indicator from rpm-250 to rpm
    kids.append(b.meter(200, 40, 400, tacho_scales([ind_arc('rpm - 250', 'rpm', 24, ORANGE, -4)])))
    kids.append(b.panel(300, 140, 200, 200, '#000000', (3, WHITE), 100, [
        b.label('km/h', 0, 30, 194, 16, GREY, 'CENTER'),
        b.value('vehicle_speed_text', 47, 62, 100, 48),
        b.value('rpm_text', 52, 130, 90, 20, GREY),
    ]))
    return kids


def page_needle(b):
    kids = []
    left = [('MAP', 'map_text'), ('IAT', 'intake_temp_text'),
            ('BATT', 'batt_volt_text'), ('Lambda', 'lam1_text')]
    right = [('Gear', 'current_gear_text'), ('Water', 'coolant_temp_text'),
             ('Oil P', 'oil_press_text'), ('Oil T', 'oil_temperature_text')]
    for col, x in ((left, 10), (right, 615)):
        for i, (name, var) in enumerate(col):
            y = 50 + i * 100
            kids.append(b.label(name, x, y + 14, 80, 20, ORANGE))
            kids.append(b.value(var, x + 80, y + 4, 95, 30))
            kids.append(b.panel(x, y + 60, 175, 2, '#404040'))
    kids.append(b.meter(200, 40, 400, tacho_scales([ind_needle('rpm', 5, RED, -6)])))
    # the hub covers the inner part of the needle (it is still redrawn underneath)
    kids.append(b.panel(330, 170, 140, 140, '#000000', (2, ORANGE), 70, [
        b.label('RPM', 0, 22, 136, 16, ORANGE, 'CENTER'),
        b.value('rpm_text', 18, 50, 100, 30),
    ]))
    return kids


def page_bars(b):
    rows = [('BOOST', 'map_val', 0, 250, 'map_text', 'kPa'),
            ('OIL-TEMP', 'oil_temperature_val', 0, 150, 'oil_temperature_text', '°C'),
            ('W-TEMP', 'coolant_temp_val', 0, 130, 'coolant_temp_text', '°C'),
            ('OIL-PRESS', 'oil_press_val', 0, 700, 'oil_press_text', 'kPa'),
            ('FUEL-PRESS', 'fp_low_val', 0, 600, 'fp_low_text', 'kPa'),
            ('INT-TEMP', 'intake_temp_val', -20, 80, 'intake_temp_text', '°C'),
            ('THROTTLE', 'tps1_val', 0, 100, 'tps1_text', '%'),
            ('BATTERY', 'batt_volt_x10', 100, 160, 'batt_volt_text', 'V'),
            ('LAMBDA', 'lam1_x100', 60, 140, 'lam1_text', '')]
    kids = []
    for i, (name, var, lo, hi, text, unit) in enumerate(rows):
        y = 6 + i * 52
        kids.append(b.panel(8, y, 200, 46, '#000000', (2, WHITE), 0, [
            b.label(name, 8, 10, 184, 22, WHITE)]))
        kids.append(b.panel(214, y, 400, 46, '#000000', (2, WHITE), 0, [
            b.bar(var, lo, hi, 6, 8, 384, 26, '#c8f000', '#1a1a1a')]))
        kids.append(b.panel(620, y, 172, 46, '#000000', (2, WHITE), 0, [
            b.value(text, 4, 7, 100, 26, WHITE, 'RIGHT'),
            b.label(unit, 112, 11, 56, 20, WHITE)]))
    return kids


def page_gauges(b):
    g = [('INTAKE TEMP', 'intake_temp_val', 0, 120, 'intake_temp_text', '°C', (0, 120, 7, 2)),
         ('COOLANT', 'coolant_temp_val', 0, 120, 'coolant_temp_text', '°C', (0, 120, 7, 2)),
         ('MAP', 'map_val', 0, 300, 'map_text', 'kPa', (0, 300, 7, 2)),
         ('BATTERY', 'batt_volt_x10', 0, 200, 'batt_volt_text', 'V', (0, 20, 5, 1)),
         ('OIL TEMP', 'oil_temperature_val', 0, 150, 'oil_temperature_text', '°C', (0, 150, 7, 2)),
         ('OIL PRESS', 'oil_press_val', 0, 700, 'oil_press_text', 'kPa', (0, 700, 8, 1))]
    kids = []
    for i, (name, var, lo, hi, text, unit, (smin, smax, sticks, every)) in enumerate(g):
        x, y = 8 + (i % 3) * 264, 8 + (i // 3) * 236
        size = 196
        gx = (256 - size) // 2
        sc = [scale(smin, smax, sticks, 2, 8, '#80c0ff', angle=240, rotation=150,
                    major_every=every, major_w=2, major_len=8, major_color='#80c0ff', label_gap=26)]
        kids.append(b.panel(x, y, 256, 228, '#1c2430', (1, '#34404f'), 6, [
            b.arc(var, lo, hi, gx, 8, size, 22, '#3fb0ff', '#26303d'),
            b.meter(gx, 8, size, sc, 14, '#80c0ff'),
            b.label(unit, 0, 68, 252, 20, WHITE, 'CENTER'),
            b.value(text, 63, 92, 126, 40, YELLOW),
            b.label(name, 0, 190, 252, 16, WHITE, 'CENTER'),
        ]))
    return kids


def page_strip(b):
    kids = [b.bar('rpm', 0, 8000, 10, 44, 780, 56, '#e0e0e0', '#303030', 6)]
    # shift lights: a grey dot with a coloured dot on top, shown above its rpm
    for i in range(8):
        x = 220 + i * 46
        rpm_on = 4000 + i * 500
        color = '#30d050' if i < 3 else (YELLOW if i < 6 else RED)
        kids.append(b.panel(x, 8, 28, 28, '#202020', (2, '#606060'), 14))
        kids.append(b.panel(x, 8, 28, 28, color, (2, '#606060'), 14, hidden=f'rpm < {rpm_on}'))
    kids += [b.label('RPM', 60, 140, 60, 16, GREY),
             b.value('rpm_text', 120, 118, 140, 48, WHITE, 'RIGHT'),
             b.label('km/h', 560, 140, 60, 16, GREY),
             b.value('vehicle_speed_text', 620, 118, 110, 48, WHITE, 'RIGHT')]
    cells = [('Oil Pres', 'oil_press_text', 'kPa', 'oil_press_val', 0, 700),
             ('Boost', 'map_text', 'kPa', 'map_val', 0, 250),
             ('Oil Temp.', 'oil_temperature_text', '°C', 'oil_temperature_val', 0, 150),
             ('Coolant', 'coolant_temp_text', '°C', 'coolant_temp_val', 0, 130),
             ('Lambda 1', 'lam1_text', '', 'lam1_x100', 60, 140),
             ('Battery Voltage', 'batt_volt_text', 'V', 'batt_volt_x10', 100, 160)]
    for i, (name, text, unit, var, lo, hi) in enumerate(cells):
        x, y = 20 + (i % 3) * 262, 200 + (i // 3) * 140
        kids += [b.label(name, x, y, 230, 22, '#ff5a30', 'CENTER'),
                 b.value(text, x + 55, y + 32, 120, 30),
                 b.label(unit, x + 180, y + 42, 50, 16, GREY),
                 b.bar(var, lo, hi, x + 10, y + 82, 210, 12, '#3070ff', '#303030')]
    return kids


def main():
    with open(PROJECT, encoding='utf-8') as f:
        project = json.load(f)
    b = Builder(project)

    gv = project['variables']['globalVariables']
    names = {v['name'] for v in gv}
    for name, (vtype, default) in NEW_VARS.items():
        if name not in names:
            gv.append({'objID': uid(), 'name': name, 'type': vtype,
                       'defaultValue': default, 'persistent': False})

    pages = [p for p in project['userPages'] if p['name'] not in PAGES]
    builders = {'ex_tacho': page_tacho, 'ex_needle': page_needle, 'ex_bars': page_bars,
                'ex_gauges': page_gauges, 'ex_strip': page_strip}
    new = []
    for name in PAGES:
        pg = copy.deepcopy(b.screen_tpl)
        scr = pg['components'][0]
        scr['children'] = builders[name](b)
        children = scr.pop('children')
        fresh_ids(pg)
        scr['children'] = children
        pg['name'] = name
        new.append(pg)
    # before "settings", so the swipe order is: own pages, examples, (settings by long press)
    at = next((i for i, p in enumerate(pages) if p['name'] == 'settings'), len(pages))
    project['userPages'] = pages[:at] + new + pages[at:]

    with open(PROJECT, 'w', encoding='utf-8') as f:
        f.write(json.dumps(project, indent=2, ensure_ascii=False))
    print('pages', ', '.join(PAGES), 'written')


if __name__ == '__main__':
    main()

"""The signal map: model, auto-mapping, XML read/write.

The XML written here is what the firmware reads (components/canmap/canmap.c).
It is self contained (bit positions, scaling, UI bindings), so it can be
opened again without the DBC, and merged with a newer DBC / EEZ project.
"""
import math
import re
import xml.etree.ElementTree as ET
from dataclasses import dataclass, field

MAP_VERSION = 1
DEFAULT_BITRATE = 500000
DEFAULT_TIMEOUT_MS = 1000


@dataclass
class UiBinding:
    var: str
    type: str = 'string'        # integer | float | string | boolean
    scale: float = 1.0
    offset: float = 0.0
    format: str = ''            # string vars only, '' = from decimals


@dataclass
class Signal:
    name: str                   # channel name, unique in the map
    start: int
    length: int
    motorola: bool
    value_type: str             # unsigned | signed | float
    factor: float
    offset: float
    unit: str = ''
    decimals: int = 0
    log: bool = True
    enabled: bool = True        # False: not written to the map (not decoded)
    comment: str = ''
    multiplex: str = ''
    ui: list = field(default_factory=list)


@dataclass
class Frame:
    can_id: int
    ext: bool
    name: str
    timeout_ms: int = DEFAULT_TIMEOUT_MS
    comment: str = ''
    signals: list = field(default_factory=list)

    @property
    def key(self):
        return (self.ext, self.can_id)

    @property
    def id_text(self):
        return f'0x{self.can_id:08X}' if self.ext else f'0x{self.can_id:03X}'


@dataclass
class SignalMap:
    frames: list = field(default_factory=list)
    bitrate: int = DEFAULT_BITRATE
    source: str = ''

    def all_signals(self):
        for fr in self.frames:
            for s in fr.signals:
                yield fr, s

    def frame(self, key):
        for fr in self.frames:
            if fr.key == key:
                return fr
        return None


def guess_decimals(factor):
    f = abs(float(factor))
    if f == 0 or f >= 1 and float(f).is_integer():
        return 0
    return max(0, min(4, math.ceil(-math.log10(f) - 1e-9)))


# ---------------------------------------------------------------- from DBC

def from_dbc(dbc_frames, source=''):
    m = SignalMap(source=source)
    seen = {}
    for df in dbc_frames:
        fr = Frame(df.can_id, df.ext, df.name, comment=df.comment)
        for ds in df.signals:
            s = Signal(name=ds.name, start=ds.start, length=ds.length, motorola=ds.motorola,
                       value_type=ds.value_type, factor=ds.factor, offset=ds.offset,
                       unit=ds.unit, decimals=guess_decimals(ds.factor), comment=ds.comment,
                       multiplex=ds.multiplex)
            if ds.multiplex:
                s.enabled = False       # not supported by the firmware yet
            fr.signals.append(s)
            seen.setdefault(ds.name, []).append(s)
        m.frames.append(fr)
    # channel names must be unique: prefix duplicates with the frame name
    for name, sigs in seen.items():
        if len(sigs) > 1:
            for fr in m.frames:
                for s in fr.signals:
                    if s in sigs:
                        s.name = f'{fr.name}_{name}'
    return m


def merge(base, new):
    """Applies the user settings of `base` (log, unit, decimals, UI bindings...)
    to the signals of `new` (fresh from a DBC). Matched by frame id + name."""
    old = {(fr.key, s.name): (fr, s) for fr, s in base.all_signals()}
    for fr in new.frames:
        bf = base.frame(fr.key)
        if bf:
            fr.timeout_ms = bf.timeout_ms
        for s in fr.signals:
            hit = old.get((fr.key, s.name))
            if hit:
                _, bs = hit
                s.unit, s.decimals, s.log, s.enabled = bs.unit, bs.decimals, bs.log, bs.enabled
                s.ui = [UiBinding(**vars(u)) for u in bs.ui]
    new.bitrate = base.bitrate
    return new


# ---------------------------------------------------------------- auto mapping

def _norm(s):
    return re.sub(r'[^a-z0-9]', '', s.lower())


def auto_map(m, eez_vars, replace=False):
    """Binds EEZ variables to signals by name:
         var 'rpm'          <-> signal 'RPM'          (any type)
         var 'oil_press_text' <-> signal 'OilPress'   (suffix _text / _str: string)
    Returns the list of (var, signal) pairs added."""
    by_norm = {}
    for fr, s in m.all_signals():
        by_norm.setdefault(_norm(s.name), s)
    added = []
    bound = {u.var for _, s in m.all_signals() for u in s.ui}
    for v in eez_vars:
        if not v.supported or v.system:
            continue
        if v.name in bound and not replace:
            continue
        base = re.sub(r'_(text|str)$', '', v.name)
        s = by_norm.get(_norm(base))
        if s is None or not s.enabled:
            continue
        if replace:
            for _, other in m.all_signals():
                other.ui = [u for u in other.ui if u.var != v.name]
        s.ui.append(UiBinding(var=v.name, type=v.type))
        added.append((v.name, s.name))
    return added


def check(m, eez_vars):
    """Returns a list of warning strings."""
    warnings = []
    known = {v.name: v for v in eez_vars}
    used = {}
    for fr, s in m.all_signals():
        if s.multiplex and s.enabled:
            warnings.append(f'{s.name}: multiplexed signals are not decoded by the firmware')
        for u in s.ui:
            used.setdefault(u.var, []).append(s.name)
            v = known.get(u.var)
            if eez_vars and v is None:
                warnings.append(f'{s.name}: UI variable "{u.var}" is not in the EEZ project')
            elif v is not None and v.type != u.type:
                warnings.append(f'{s.name}: "{u.var}" is {v.type} in EEZ but {u.type} in the map')
            if u.format and u.type != 'string':
                warnings.append(f'{s.name}: format only applies to string variables ("{u.var}")')
        if s.ui and not s.enabled:
            warnings.append(f'{s.name}: has UI bindings but is not included in the map')
    for var, sigs in used.items():
        if len(sigs) > 1:
            warnings.append(f'"{var}" is fed by several signals: {", ".join(sigs)}')
    for v in eez_vars:
        if v.supported and not v.system and v.name not in used:
            warnings.append(f'EEZ variable "{v.name}" is not mapped (keeps its default value)')
    return warnings


# ---------------------------------------------------------------- XML

def _fmt_num(x):
    x = float(x)
    if x.is_integer():
        return str(int(x))
    return repr(x)


def write_xml(m, path):
    root = ET.Element('fpdash_map', {'version': str(MAP_VERSION)})
    if m.source:
        root.set('source', m.source)
    ET.SubElement(root, 'bus', {'bitrate': str(m.bitrate)})
    for fr in m.frames:
        sigs = [s for s in fr.signals if s.enabled]
        if not sigs:
            continue
        fe = ET.SubElement(root, 'frame', {
            'id': fr.id_text, 'ext': '1' if fr.ext else '0', 'name': fr.name,
            'timeout_ms': str(fr.timeout_ms)})
        for s in sigs:
            se = ET.SubElement(fe, 'signal', {
                'name': s.name, 'start': str(s.start), 'len': str(s.length),
                'order': 'motorola' if s.motorola else 'intel', 'type': s.value_type,
                'factor': _fmt_num(s.factor), 'offset': _fmt_num(s.offset),
                'unit': s.unit, 'decimals': str(s.decimals), 'log': '1' if s.log else '0'})
            for u in s.ui:
                a = {'var': u.var, 'type': u.type}
                if u.scale != 1.0:
                    a['scale'] = _fmt_num(u.scale)
                if u.offset != 0.0:
                    a['offset'] = _fmt_num(u.offset)
                if u.format and u.type == 'string':
                    a['format'] = u.format
                ET.SubElement(se, 'ui', a)
    ET.indent(root, space='  ')
    body = ET.tostring(root, encoding='unicode')
    with open(path, 'w', encoding='utf-8', newline='\n') as f:
        f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
        f.write('<!-- FP_Dash signal map, written by tools/fpdash_mapper. Copy to /sdcard/fpdash/map.xml -->\n')
        f.write(body)
        f.write('\n')


def read_xml(path):
    root = ET.parse(path).getroot()
    if root.tag != 'fpdash_map' or root.get('version') != str(MAP_VERSION):
        raise ValueError(f'{path}: not an fpdash_map version {MAP_VERSION} file')
    m = SignalMap(source=root.get('source', ''))
    bus = root.find('bus')
    if bus is not None and bus.get('bitrate'):
        m.bitrate = int(bus.get('bitrate'), 0)
    for fe in root.findall('frame'):
        fr = Frame(int(fe.get('id'), 0), fe.get('ext', '0') == '1', fe.get('name', ''),
                   int(fe.get('timeout_ms', DEFAULT_TIMEOUT_MS)))
        for se in fe.findall('signal'):
            s = Signal(name=se.get('name'), start=int(se.get('start')), length=int(se.get('len')),
                       motorola=se.get('order', 'intel') == 'motorola',
                       value_type=se.get('type', 'unsigned'),
                       factor=float(se.get('factor', '1')), offset=float(se.get('offset', '0')),
                       unit=se.get('unit', ''), decimals=int(se.get('decimals', '0')),
                       log=se.get('log', '1') != '0')
            for ue in se.findall('ui'):
                s.ui.append(UiBinding(var=ue.get('var'), type=ue.get('type', 'string'),
                                      scale=float(ue.get('scale', '1')),
                                      offset=float(ue.get('offset', '0')),
                                      format=ue.get('format', '')))
            fr.signals.append(s)
        m.frames.append(fr)
    return m

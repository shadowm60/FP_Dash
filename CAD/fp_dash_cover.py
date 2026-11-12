# FP_Dash back cover for the Waveshare ESP32-S3-Touch-LCD-7 (FreeCAD 1.x macro)
#
# Builds FP_Dash_Cover.FCStd: a parametric back cover + service hatch, plus
# "keep-out" reference bodies of the display module (not printed). Every size
# and position is a cell of the spreadsheet "Params"; the features read them
# through expressions, so editing a cell and pressing Recompute (Ctrl+R)
# updates the model.
#
#   GUI:      Macro > Macros... > fp_dash_cover.py > Execute
#   headless: freecad.cmd CAD/fp_dash_cover.py      (also exports STL / STEP)
#
# Coordinates, seen from BEHIND (as the Waveshare back-view drawing):
#   origin = bottom-left corner of the glass, on the glass back face
#   X right, Y up, Z out of the back (away from the glass)
#
# Structural choices (which features exist) are the switches below; sizes are
# in the spreadsheet. See CAD/README.md.

import math
import os

import FreeCAD as App
import Part

BUILD_BATTERY_BAY = True    # frame for a 1-cell LiPo pouch (see README: heat!)
BUILD_BUCK_CRADLE = True    # rails for the 12 V -> 5 V converter module
BUILD_VENTS = True          # slots in the back

DOC_NAME = "FP_Dash_Cover"

try:
    HERE = os.path.dirname(os.path.abspath(__file__))
except NameError:
    HERE = os.getcwd()

# ----------------------------------------------------------------- parameters
# (alias, value, unit, comment). "=" values are formulas (derived, don't edit).
SECTIONS = [
    ("Display module - from the Waveshare drawing, CHECK WITH CALIPERS", [
        ("glass_w", 192.96, "mm", "glass width"),
        ("glass_h", 110.76, "mm", "glass height"),
        ("glass_r", 8.0, "mm", "glass corner radius (MEASURE)"),
        ("glass_t", 2.0, "mm", "glass thickness, reference only (MEASURE)"),
        ("lcd_w", 165.72, "mm", "LCD metal back plate width"),
        ("lcd_h", 97.60, "mm", "LCD metal back plate height"),
        ("lcd_x", 13.62, "mm", "glass left edge -> plate left edge, seen from behind (MEASURE)"),
        ("lcd_y", 9.0, "mm", "glass bottom edge -> plate bottom edge (MEASURE)"),
        ("lcd_t", 5.0, "mm", "glass back face -> plate surface (MEASURE)"),
        ("so_x0", 19.76, "mm", "plate left edge -> M3 standoff column 1"),
        ("so_dx", 126.20, "mm", "M3 standoff spacing X"),
        ("so_top", 18.31, "mm", "plate top edge -> upper M3 standoff row"),
        ("so_dy", 65.65, "mm", "M3 standoff spacing Y"),
        ("so_h", 5.0, "mm", "M3 standoff height above the plate (MEASURE)"),
        ("pcb_x", 25.9, "mm", "plate left edge -> PCB left edge (estimated)"),
        ("pcb_y", 0.0, "mm", "plate bottom edge -> PCB bottom edge (estimated)"),
        ("pcb_w", 108.6, "mm", "PCB width (estimated)"),
        ("pcb_h", 74.0, "mm", "PCB height (estimated)"),
        ("pcb_stack", 14.0, "mm", "plate -> top of the tallest part on the PCB (MEASURE)"),
    ]),
    ("Cover", [
        ("depth_in", 24.0, "mm", "inside depth: plate surface -> inner back face"),
        ("wall", 2.4, "mm", "side wall thickness (6 lines of 0.4 nozzle)"),
        ("back_t", 2.5, "mm", "back thickness"),
        ("rim_gap", 0.5, "mm", "gap rim <-> glass, filled by ~1 mm foam tape"),
        ("post_d", 7.0, "mm", "screw post diameter"),
        ("screw_d", 3.4, "mm", "M3 clearance hole"),
    ]),
    ("Service hatch (UART + USB USB-C, SD card) - window in plate coordinates", [
        ("win_x", 2.0, "mm", "plate left edge -> window left edge"),
        ("win_top", 26.0, "mm", "plate top edge -> window top edge (screw head above!)"),
        ("win_w", 60.0, "mm", "window width"),
        ("win_h", 40.0, "mm", "window height"),
        ("hatch_t", 2.0, "mm", "hatch thickness"),
        ("hatch_ovl", 4.0, "mm", "hatch overlap around the window"),
        ("hatch_boss_d", 7.0, "mm", "hatch screw boss diameter"),
        ("hatch_boss_h", 5.0, "mm", "hatch screw boss height (inside)"),
        ("pilot_d", 2.6, "mm", "pilot hole for an M3 screw cut into plastic"),
    ]),
    ("Car connector (4 pin: KL15, KL31, CAN-H, CAN-L) in the bottom wall", [
        ("con_d", 12.2, "mm", "panel hole: GX12 = 12.2, GX16 / M16 = 16.2"),
        ("con_x", 172.0, "mm", "connector centre X (glass coordinates)"),
        ("con_z_from_back", 10.0, "mm", "inner back face -> connector axis"),
        ("con_body_d", 16.0, "mm", "nut / body diameter inside (keep-out check)"),
        ("con_body_l", 22.0, "mm", "body + solder cups length inside (keep-out check)"),
        ("head_d", 6.0, "mm", "M3 screw head diameter (keep-out check)"),
    ]),
    ("Mount: AMPS pattern on the back (RAM / phone mounts)", [
        ("amps_cx", 110.0, "mm", "pattern centre X (glass coordinates)"),
        ("amps_cy", 60.0, "mm", "pattern centre Y"),
        ("amps_sx", 38.0, "mm", "hole spacing X"),
        ("amps_sy", 30.0, "mm", "hole spacing Y"),
        ("amps_d", 4.3, "mm", "M4 clearance hole"),
        ("amps_boss_d", 11.0, "mm", "boss diameter (inside)"),
        ("amps_boss_h", 4.0, "mm", "boss height (inside)"),
        ("nut_af", 7.0, "mm", "M4 nut across flats"),
        ("nut_tol", 0.3, "mm", "nut trap clearance"),
        ("nut_depth", 3.4, "mm", "nut trap depth"),
    ]),
    ("12 V -> 5 V converter cradle (e.g. LM2596 module 43 x 21 mm)", [
        ("buck_w", 22.0, "mm", "module width (X)"),
        ("buck_l", 45.0, "mm", "module length (Y)"),
        ("buck_cx", 175.0, "mm", "cradle centre X (glass coordinates)"),
        ("buck_cy", 55.5, "mm", "cradle centre Y"),
        ("rail_t", 1.6, "mm", "rail thickness"),
        ("rail_h", 5.0, "mm", "rail height"),
        ("fit", 0.4, "mm", "clearance module <-> rails, each side"),
        ("buck_h", 15.0, "mm", "module height incl. parts (keep-out check)"),
    ]),
    ("Battery bay (1-cell LiPo pouch, e.g. 503450 = 5 x 34 x 50 mm)", [
        ("bat_l", 50.0, "mm", "length (X)"),
        ("bat_w", 34.0, "mm", "width (Y)"),
        ("bat_t", 6.0, "mm", "thickness incl. swelling margin"),
        ("bat_cx", 65.0, "mm", "centre X (glass coordinates)"),
        ("bat_cy", 19.5, "mm", "centre Y"),
        ("bat_frame_h", 3.0, "mm", "frame height"),
        ("bat_gap", 10.0, "mm", "opening in the frame for the wires"),
    ]),
    ("Vents", [
        ("vent_w", 2.5, "mm", "slot width"),
        ("vent_l", 14.0, "mm", "slot length"),
        ("vent_pitch", 8.0, "mm", "slot spacing"),
        ("vent_cx", 96.48, "mm", "row centre X"),
        ("vent_cy", 98.0, "mm", "row centre Y"),
    ]),
    ("Derived - formulas, do not edit", [
        ("z_back_in", "=lcd_t + depth_in", "", "Z of the inner back face"),
        ("z_top", "=z_back_in + back_t", "", "Z of the outer back face"),
        ("lcd_top_y", "=lcd_y + lcd_h", "", "Y of the plate top edge"),
        ("so_x1", "=lcd_x + so_x0", "", "standoff column 1 X"),
        ("so_x2", "=so_x1 + so_dx", "", "standoff column 2 X"),
        ("so_y1", "=lcd_top_y - so_top", "", "upper standoff row Y"),
        ("so_y2", "=so_y1 - so_dy", "", "lower standoff row Y"),
        ("so_z", "=lcd_t + so_h", "", "Z of the standoff tops"),
        ("post_len", "=z_back_in - so_z", "", "screw post length"),
        ("screw_len", "=back_t + post_len + 4mm", "", "M3 screw length needed (round up)"),
        ("win_ax", "=lcd_x + win_x", "", "window X (glass coordinates)"),
        ("win_ay", "=lcd_top_y - win_top - win_h", "", "window Y (glass coordinates)"),
        ("clear_left", "=lcd_x - wall", "", "gap wall <-> plate, left (> 0.5)"),
        ("clear_right", "=glass_w - lcd_x - lcd_w - wall", "", "gap wall <-> plate, right"),
        ("clear_bottom", "=lcd_y - wall", "", "gap wall <-> plate, bottom"),
        ("clear_top", "=glass_h - lcd_top_y - wall", "", "gap wall <-> plate, top"),
        ("clear_pcb", "=depth_in - pcb_stack", "", "gap PCB parts <-> back (> 2)"),
        ("clear_bat", "=depth_in - bat_t - pcb_stack", "", "gap PCB parts <-> battery"),
    ]),
]


def make_params(doc):
    ss = doc.addObject("Spreadsheet::Sheet", "Params")
    ss.set("A1", "Parameter")
    ss.set("B1", "Value")
    ss.set("C1", "Meaning")
    row = 2
    for title, rows in SECTIONS:
        row += 1
        ss.set(f"A{row}", title)
        row += 1
        for alias, value, unit, comment in rows:
            ss.set(f"A{row}", alias)
            if isinstance(value, str):
                ss.set(f"B{row}", value)
            else:
                ss.set(f"B{row}", f"{value} {unit}".strip())
            ss.setAlias(f"B{row}", alias)
            ss.set(f"C{row}", comment)
            row += 1
    ss.setColumnWidth("A", 160)
    ss.setColumnWidth("B", 90)
    ss.setColumnWidth("C", 480)
    doc.recompute()
    return ss


# ----------------------------------------------------------------- helpers
# Sizes / positions are expression strings; "p." is replaced by "Params.".

def q(v):
    """Plain numbers are millimetres (FreeCAD won't add 0 to a length)."""
    return f"{v} mm" if isinstance(v, (int, float)) else v


def ex(e):
    return str(q(e)).replace("p.", "Params.")


def set_pos(obj, x, y, z):
    obj.setExpression("Placement.Base.x", ex(x))
    obj.setExpression("Placement.Base.y", ex(y))
    obj.setExpression("Placement.Base.z", ex(z))


def box(doc, name, L, W, H, x, y, z):
    o = doc.addObject("Part::Box", name)
    o.setExpression("Length", ex(L))
    o.setExpression("Width", ex(W))
    o.setExpression("Height", ex(H))
    set_pos(o, x, y, z)
    return o


def cyl(doc, name, d, H, x, y, z, axis="z"):
    """Cylinder of diameter d, height H; base centre at (x, y, z).
    axis "z" (default) or "y" (horizontal, pointing +Y)."""
    o = doc.addObject("Part::Cylinder", name)
    o.setExpression("Radius", ex(f"({d}) / 2"))
    o.setExpression("Height", ex(H))
    if axis == "y":
        o.Placement.Rotation = App.Rotation(App.Vector(1, 0, 0), -90)
    set_pos(o, x, y, z)
    return o


def hexagon(doc, name, af, H, x, y, z):
    """Hexagonal prism (nut trap), across flats af."""
    o = doc.addObject("Part::Prism", name)
    o.Polygon = 6
    o.setExpression("Circumradius", ex(f"({af}) / sqrt(3)"))
    o.setExpression("Height", ex(H))
    set_pos(o, x, y, z)
    return o


def fuse(doc, name, shapes, refine=True):
    o = doc.addObject("Part::MultiFuse", name)
    o.Shapes = shapes
    o.Refine = refine
    return o


def cut(doc, name, base, tool, refine=True):
    o = doc.addObject("Part::Cut", name)
    o.Base = base
    o.Tool = tool
    o.Refine = refine
    return o


def rbox(doc, name, x, y, z, w, h, t, r):
    """Box with rounded vertical edges = 2 boxes + 4 cylinders."""
    x, y, z, w, h, t, r = (q(v) for v in (x, y, z, w, h, t, r))
    parts = [
        box(doc, f"{name}_bx", f"({w}) - 2 * ({r})", h, t, f"({x}) + ({r})", y, z),
        box(doc, f"{name}_by", w, f"({h}) - 2 * ({r})", t, x, f"({y}) + ({r})", z),
    ]
    for i, (cx, cy) in enumerate([
            (f"({x}) + ({r})", f"({y}) + ({r})"),
            (f"({x}) + ({w}) - ({r})", f"({y}) + ({r})"),
            (f"({x}) + ({r})", f"({y}) + ({h}) - ({r})"),
            (f"({x}) + ({w}) - ({r})", f"({y}) + ({h}) - ({r})")]):
        parts.append(cyl(doc, f"{name}_c{i}", f"2 * ({r})", t, cx, cy, z))
    return fuse(doc, name, parts)


def group(doc, name, objs):
    g = doc.addObject("App::DocumentObjectGroup", name)
    for o in objs:
        g.addObject(o)
    return g


# ----------------------------------------------------------------- model

def build():
    if App.listDocuments().get(DOC_NAME):
        App.closeDocument(DOC_NAME)
    doc = App.newDocument(DOC_NAME)
    make_params(doc)

    # --- reference: the display module (keep-out volumes, not printed)
    ref = [
        box(doc, "Ref_Glass", "p.glass_w", "p.glass_h", "p.glass_t", 0, 0, "-p.glass_t"),
        box(doc, "Ref_LCD", "p.lcd_w", "p.lcd_h", "p.lcd_t", "p.lcd_x", "p.lcd_y", 0),
        box(doc, "Ref_PCB_parts", "p.pcb_w", "p.pcb_h", "p.pcb_stack",
            "p.lcd_x + p.pcb_x", "p.lcd_y + p.pcb_y", "p.lcd_t"),
    ]
    for i, (sx, sy) in enumerate([("p.so_x1", "p.so_y1"), ("p.so_x2", "p.so_y1"),
                                  ("p.so_x1", "p.so_y2"), ("p.so_x2", "p.so_y2")]):
        ref.append(cyl(doc, f"Ref_Standoff{i}", "6mm", "p.so_h", sx, sy, "p.lcd_t"))

    # keep-outs for the parts that go inside (checked like the display)
    ref.append(box(doc, "Ref_Connector_body", "p.con_body_d", "p.con_body_l", "p.con_body_d",
                   "p.con_x - p.con_body_d / 2", "p.wall",
                   "p.z_back_in - p.con_z_from_back - p.con_body_d / 2"))
    if BUILD_BUCK_CRADLE:
        ref.append(box(doc, "Ref_Buck_module", "p.buck_w", "p.buck_l", "p.buck_h",
                       "p.buck_cx - p.buck_w / 2", "p.buck_cy - p.buck_l / 2",
                       "p.z_back_in - p.buck_h"))
    if BUILD_BATTERY_BAY:
        ref.append(box(doc, "Ref_Battery", "p.bat_l", "p.bat_w", "p.bat_t",
                       "p.bat_cx - p.bat_l / 2", "p.bat_cy - p.bat_w / 2", "p.z_back_in - p.bat_t"))

    # --- cover: shell
    shell_out = rbox(doc, "Shell_outer", 0, 0, "p.rim_gap", "p.glass_w", "p.glass_h",
                     "p.z_top - p.rim_gap", "p.glass_r")
    shell_in = rbox(doc, "Shell_inner", "p.wall", "p.wall", "p.rim_gap - 1mm",
                    "p.glass_w - 2 * p.wall", "p.glass_h - 2 * p.wall",
                    "p.z_back_in - p.rim_gap + 1mm", "p.glass_r - p.wall")
    shell = cut(doc, "Shell", shell_out, shell_in)

    adds = [shell]          # solids fused to the shell
    holes = []              # solids cut from the result

    # screw posts onto the M3 standoffs
    so = [("p.so_x1", "p.so_y1"), ("p.so_x2", "p.so_y1"),
          ("p.so_x1", "p.so_y2"), ("p.so_x2", "p.so_y2")]
    for i, (sx, sy) in enumerate(so):
        adds.append(cyl(doc, f"Post{i}", "p.post_d", "p.post_len", sx, sy, "p.so_z"))
        holes.append(cyl(doc, f"Post{i}_hole", "p.screw_d", "p.post_len + p.back_t + 2mm",
                         sx, sy, "p.so_z - 1mm"))

    # service hatch window + 2 screw bosses left / right of it
    holes.append(box(doc, "Hatch_window", "p.win_w", "p.win_h", "p.back_t + 2mm",
                     "p.win_ax", "p.win_ay", "p.z_back_in - 1mm"))
    hb = [("p.win_ax - p.hatch_boss_d / 2 - 1.5mm", "p.win_ay + p.win_h / 2"),
          ("p.win_ax + p.win_w + p.hatch_boss_d / 2 + 1.5mm", "p.win_ay + p.win_h / 2")]
    for i, (bx, by) in enumerate(hb):
        adds.append(cyl(doc, f"Hatch_boss{i}", "p.hatch_boss_d", "p.hatch_boss_h",
                        bx, by, "p.z_back_in - p.hatch_boss_h"))
        holes.append(cyl(doc, f"Hatch_pilot{i}", "p.pilot_d", "p.hatch_boss_h + p.back_t + 2mm",
                         bx, by, "p.z_back_in - p.hatch_boss_h - 1mm"))

    # car connector in the bottom wall
    holes.append(cyl(doc, "Connector_hole", "p.con_d", "p.wall + 2mm",
                     "p.con_x", "-1mm", "p.z_back_in - p.con_z_from_back", axis="y"))

    # AMPS mount: holes, bosses, M4 nut traps (nut inside, screw from the mount)
    for i, (dx, dy) in enumerate([(-1, -1), (1, -1), (-1, 1), (1, 1)]):
        ax = f"p.amps_cx + {dx} * p.amps_sx / 2"
        ay = f"p.amps_cy + {dy} * p.amps_sy / 2"
        adds.append(cyl(doc, f"AMPS_boss{i}", "p.amps_boss_d", "p.amps_boss_h",
                        ax, ay, "p.z_back_in - p.amps_boss_h"))
        holes.append(cyl(doc, f"AMPS_hole{i}", "p.amps_d", "p.amps_boss_h + p.back_t + 2mm",
                         ax, ay, "p.z_back_in - p.amps_boss_h - 1mm"))
        holes.append(hexagon(doc, f"AMPS_nut{i}", "p.nut_af + p.nut_tol", "p.nut_depth + 1mm",
                             ax, ay, "p.z_back_in - p.amps_boss_h - 1mm"))

    # 12 V -> 5 V converter cradle: 2 rails + corner stops at both ends
    if BUILD_BUCK_CRADLE:
        inner_w = "(p.buck_w + 2 * p.fit)"
        rail_len = "(p.buck_l + 2 * p.fit + 2 * p.rail_t)"
        y0 = f"p.buck_cy - {rail_len} / 2"
        z0 = "p.z_back_in - p.rail_h"
        adds.append(box(doc, "Buck_rail_L", "p.rail_t", rail_len, "p.rail_h",
                        f"p.buck_cx - {inner_w} / 2 - p.rail_t", y0, z0))
        adds.append(box(doc, "Buck_rail_R", "p.rail_t", rail_len, "p.rail_h",
                        f"p.buck_cx + {inner_w} / 2", y0, z0))
        for i, (sx, sy) in enumerate([
                (f"p.buck_cx - {inner_w} / 2", y0),
                (f"p.buck_cx + {inner_w} / 2 - 4mm", y0),
                (f"p.buck_cx - {inner_w} / 2", f"p.buck_cy + {rail_len} / 2 - p.rail_t"),
                (f"p.buck_cx + {inner_w} / 2 - 4mm", f"p.buck_cy + {rail_len} / 2 - p.rail_t")]):
            adds.append(box(doc, f"Buck_stop{i}", "4mm", "p.rail_t", "p.rail_h", sx, sy, z0))

    # battery bay: frame with an opening for the wires on the right side
    if BUILD_BATTERY_BAY:
        iw = "(p.bat_l + 2 * p.fit)"
        ih = "(p.bat_w + 2 * p.fit)"
        x0 = f"p.bat_cx - {iw} / 2 - p.rail_t"
        y0 = f"p.bat_cy - {ih} / 2 - p.rail_t"
        z0 = "p.z_back_in - p.bat_frame_h"
        ow = f"({iw} + 2 * p.rail_t)"
        oh = f"({ih} + 2 * p.rail_t)"
        seg = f"(({ih} - p.bat_gap) / 2)"
        adds += [
            box(doc, "Bat_frame_B", ow, "p.rail_t", "p.bat_frame_h", x0, y0, z0),
            box(doc, "Bat_frame_T", ow, "p.rail_t", "p.bat_frame_h", x0, f"{y0} + {oh} - p.rail_t", z0),
            box(doc, "Bat_frame_L", "p.rail_t", oh, "p.bat_frame_h", x0, y0, z0),
            box(doc, "Bat_frame_R1", "p.rail_t", f"{seg} + p.rail_t", "p.bat_frame_h",
                f"{x0} + {ow} - p.rail_t", y0, z0),
            box(doc, "Bat_frame_R2", "p.rail_t", f"{seg} + p.rail_t", "p.bat_frame_h",
                f"{x0} + {ow} - p.rail_t", f"{y0} + {oh} - p.rail_t - {seg}", z0),
        ]

    # vent slots in the back
    if BUILD_VENTS:
        n = 10                              # number of slots
        for i in range(n):
            holes.append(box(doc, f"Vent{i}", "p.vent_w", "p.vent_l", "p.back_t + 2mm",
                             f"p.vent_cx + ({i} - ({n} - 1) / 2) * p.vent_pitch - p.vent_w / 2",
                             "p.vent_cy - p.vent_l / 2", "p.z_back_in - 1mm"))

    body = fuse(doc, "Cover_body", adds)
    cutters = fuse(doc, "Cover_holes", holes, refine=False)
    cover = cut(doc, "Cover", body, cutters)

    # --- hatch (separate print), sits on the outer back face
    hw = "(p.win_w + 2 * p.hatch_boss_d + 6mm)"
    hh = "(p.win_h + 2 * p.hatch_ovl)"
    hatch_plate = rbox(doc, "Hatch_plate", f"p.win_ax - p.hatch_boss_d - 3mm",
                       "p.win_ay - p.hatch_ovl", "p.z_top", hw, hh, "p.hatch_t", "3mm")
    hatch_holes = [cyl(doc, f"Hatch_hole{i}", "p.screw_d", "p.hatch_t + 2mm", bx, by, "p.z_top - 1mm")
                   for i, (bx, by) in enumerate(hb)]
    hatch = cut(doc, "Hatch", hatch_plate, fuse(doc, "Hatch_holes", hatch_holes, refine=False))

    # cover screw heads on the outside: must not land on the hatch
    heads = [cyl(doc, f"Ref_Screw_head{i}", "p.head_d", "3mm", sx, sy, "p.z_top")
             for i, (sx, sy) in enumerate(so)]
    ref += heads

    doc.recompute()
    group(doc, "Reference_display", ref)
    if App.GuiUp:
        for o in ref:
            o.ViewObject.Transparency = 70
            o.ViewObject.ShapeColor = (0.3, 0.3, 0.8)
        cover.ViewObject.ShapeColor = (0.25, 0.25, 0.25)
        hatch.ViewObject.ShapeColor = (0.8, 0.4, 0.1)
    return doc, cover, hatch, ref


# ----------------------------------------------------------------- checks

def check(doc, cover, hatch, ref):
    ss = doc.getObject("Params")
    ok = True
    print("--- clearances (mm)")
    for a in ("clear_left", "clear_right", "clear_bottom", "clear_top", "clear_pcb", "clear_bat",
              "post_len", "screw_len"):
        v = float(ss.get(a))
        bad = a.startswith("clear") and v < (2.0 if a in ("clear_pcb",) else 0.5)
        if a == "clear_bat" and not BUILD_BATTERY_BAY:
            bad = False
        ok &= not bad
        print(f"  {a:14s} {v:7.2f}{'   <-- too small' if bad else ''}")
    print("--- interference (must be 0): cover / hatch <-> display module and parts inside")
    for r in ref:
        v = cover.Shape.common(r.Shape).Volume + hatch.Shape.common(r.Shape).Volume
        print(f"  {r.Name:20s} {v:8.3f} mm3{'   <-- collision' if v > 1e-3 else ''}")
        ok &= v < 1e-3
    print("--- parts inside <-> each other / PCB (must be 0)")
    inside = [r for r in ref if not r.Name.startswith(("Ref_Screw_head", "Ref_Glass"))]
    for i, a in enumerate(inside):
        for b in inside[i + 1:]:
            v = a.Shape.common(b.Shape).Volume
            if v > 1e-3:
                print(f"  {a.Name} <-> {b.Name}: {v:.1f} mm3   <-- collision")
                ok = False
    print("--- cover volume %.1f cm3, hatch %.1f cm3, valid %s" %
          (cover.Shape.Volume / 1000, hatch.Shape.Volume / 1000, cover.Shape.isValid()))
    ok &= cover.Shape.isValid() and hatch.Shape.isValid()
    print("CHECK", "OK" if ok else "FAILED")
    return ok


def export(doc, cover, hatch):
    import Mesh
    import MeshPart
    out = os.path.join(HERE, "export")
    os.makedirs(out, exist_ok=True)
    for o, name in ((cover, "fp_dash_cover"), (hatch, "fp_dash_hatch")):
        m = MeshPart.meshFromShape(Shape=o.Shape, LinearDeflection=0.05, AngularDeflection=0.2)
        m.write(os.path.join(out, name + ".stl"))
        o.Shape.exportStep(os.path.join(out, name + ".step"))
    print("exported to", out)


doc, cover, hatch, ref = build()
check(doc, cover, hatch, ref)
doc.saveAs(os.path.join(HERE, DOC_NAME + ".FCStd"))
print("saved", os.path.join(HERE, DOC_NAME + ".FCStd"))
if not App.GuiUp:
    export(doc, cover, hatch)

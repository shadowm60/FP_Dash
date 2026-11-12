# Renders PNG previews of FP_Dash_Cover.FCStd (no GUI needed):
#   freecad.cmd CAD/render_preview.py
# Writes CAD/preview/*.png. Uses matplotlib from FreeCAD's Python.
# matplotlib sorts faces roughly, so hidden parts can show through in
# oblique views: use these for a quick look, FreeCAD itself for details.
import os

import FreeCAD as App
import MeshPart
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection

try:
    HERE = os.path.dirname(os.path.abspath(__file__))
except NameError:
    HERE = os.getcwd()
OUT = os.path.join(HERE, "preview")
os.makedirs(OUT, exist_ok=True)

doc = App.openDocument(os.path.join(HERE, "FP_Dash_Cover.FCStd"))
doc.recompute()


def tris(obj, defl=0.4):
    m = MeshPart.meshFromShape(Shape=obj.Shape, LinearDeflection=defl, AngularDeflection=0.5)
    return [[tuple(p) for p in f.Points] for f in m.Facets]


def shade(t, color, alpha, light=(-0.4, -0.5, 0.75)):
    """Simple Lambert shading (two-sided): one RGBA per triangle."""
    import numpy as np
    base = np.array(matplotlib.colors.to_rgb(color))
    a = np.array(t)
    n = np.cross(a[:, 1] - a[:, 0], a[:, 2] - a[:, 0])
    n /= np.linalg.norm(n, axis=1, keepdims=True) + 1e-12
    k = 0.35 + 0.65 * np.abs(n @ (np.array(light) / np.linalg.norm(light)))
    rgb = np.clip(base[None, :] * k[:, None], 0, 1)
    return np.hstack([rgb, np.full((len(rgb), 1), alpha)])


def view(name, objs, elev, azim, title, size=(12, 7.5)):
    fig = plt.figure(figsize=size, dpi=110)
    ax = fig.add_subplot(111, projection="3d")
    for o, color, alpha in objs:
        t = tris(o)
        pc = Poly3DCollection(t, edgecolor="none")
        pc.set_facecolor(shade(t, color, alpha))
        ax.add_collection3d(pc)
    ax.set_xlim(0, 193)
    ax.set_ylim(0, 111)
    ax.set_zlim(-20, 60)
    ax.set_box_aspect((193, 111, 80))
    ax.view_init(elev=elev, azim=azim)
    ax.set_axis_off()
    ax.set_title(title)
    fig.tight_layout()
    fig.savefig(os.path.join(OUT, name), bbox_inches="tight")
    plt.close(fig)
    print("wrote", name)


cover = doc.getObject("Cover")
hatch = doc.getObject("Hatch")
ref = [doc.getObject(n) for n in ("Ref_LCD", "Ref_PCB_parts")]
grey, orange, blue, green = "#8a8a8a", "#e07020", "#4060c0", "#30a050"

view("back.png", [(cover, grey, 1.0), (hatch, orange, 1.0)], 89, -90,
     "Back (outside): hatch, AMPS mount, vents")
view("inside.png", [(cover, grey, 1.0)], -60, -90,
     "Inside (seen from the display side, mirrored)")
view("inside_with_board.png",
     [(cover, grey, 0.35), (ref[0], blue, 0.5), (ref[1], green, 0.6)], -35, -60,
     "Inside with the display module (blue) and PCB keep-out (green)")

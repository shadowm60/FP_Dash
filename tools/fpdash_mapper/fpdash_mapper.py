#!/usr/bin/env python3
"""FP_Dash signal mapper.

Maps the signals of a DBC file to the global variables of the EEZ Studio
project and writes the map XML the firmware loads from /sdcard/fpdash/map.xml.

GUI (default):
    fpdash_mapper.py [--dbc file.dbc] [--eez project.eez-project] [--map map.xml]

Command line (no GUI, e.g. to regenerate the built-in map):
    fpdash_mapper.py --dbc file.dbc --eez project.eez-project [--base old.xml] --auto -o map.xml

    --base   keep log/unit/decimals/UI settings from an earlier map (matched by
             frame id + signal name), e.g. after the DBC or the UI changed
    --auto   bind EEZ variables to signals with the same name
             ("rpm" <-> "RPM", "oil_press_text" <-> "OilPress")
    --check  print warnings and exit 1 if there are any

Only the Python standard library is needed (tkinter for the GUI).
"""
import argparse
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import dbc        # noqa: E402
import eez        # noqa: E402
import mapmodel   # noqa: E402


def build(args):
    smap = None
    if args.dbc:
        smap = mapmodel.from_dbc(dbc.load(args.dbc), os.path.basename(args.dbc))
    base = mapmodel.read_xml(args.base or args.map) if (args.base or args.map) else None
    if smap is None:
        smap = base or mapmodel.SignalMap()
    elif base is not None:
        smap = mapmodel.merge(base, smap)
    if args.bitrate:
        smap.bitrate = args.bitrate
    eez_vars = eez.load(args.eez) if args.eez else []
    return smap, eez_vars


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--dbc', help='DBC file')
    ap.add_argument('--eez', help='EEZ Studio project (.eez-project)')
    ap.add_argument('--map', help='existing map XML to open (GUI) / start from')
    ap.add_argument('--base', help='earlier map XML whose settings are kept')
    ap.add_argument('--auto', action='store_true', help='auto-map EEZ variables by name')
    ap.add_argument('--bitrate', type=int, help='CAN bitrate written to the map')
    ap.add_argument('--check', action='store_true', help='print warnings, exit 1 if any')
    ap.add_argument('-o', '--output', help='write the map XML here (no GUI)')
    args = ap.parse_args()

    try:
        smap, eez_vars = build(args)
    except (OSError, ValueError, KeyError) as e:
        print(f'error: {e}', file=sys.stderr)
        return 2

    if args.auto:
        for var, sig in mapmodel.auto_map(smap, eez_vars):
            print(f'auto-map: {var:<24} <- {sig}')

    if args.output or args.check:
        warnings = mapmodel.check(smap, eez_vars)
        for w in warnings:
            print(f'warning: {w}', file=sys.stderr)
        if args.output:
            mapmodel.write_xml(smap, args.output)
            n = sum(1 for _, s in smap.all_signals() if s.enabled)
            print(f'wrote {args.output}: {len(smap.frames)} frames, {n} signals')
        return 1 if (args.check and warnings) else 0

    import gui
    gui.run(smap=smap, eez_vars=eez_vars, map_path=args.map, dbc_path=args.dbc, eez_path=args.eez)
    return 0


if __name__ == '__main__':
    sys.exit(main())

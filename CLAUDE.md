# FP_Dash: context for Claude Code sessions

Read `README.md` first for the architecture, the logging layout and the UI
conventions. `tools/fpdash_mapper/README.md` has the map format. This file
covers the working rules and where development stopped.

## Project

- **Device:** a passive CAN dash on a **Waveshare ESP32-S3-Touch-LCD-7** (800x480).
- **Software:** ESP-IDF v6.1, LVGL 8.3, EEZ Studio 0.29 with EEZ Flow.
- **First ECU:** rusEFI broadcast (`tools/dbc/rusEFI_CAN_verbose.dbc`).
- **Sibling project:** `~/git/OBD_Dash` is an OBD-II dash on the 5" board.
  `board`, `can_if` and the datalog/EEZ setup were taken from it. Check how it
  solved something before reinventing.

## Build / UI

```sh
. ~/.espressif/tools/activate_idf_v6.1.sh
idf.py -B build build
tools/eez-build.sh          # regenerate components/hmi/src/ui from the EEZ project (needs a desktop session)
python3 tools/fpdash_mapper/fpdash_mapper.py --dbc tools/dbc/rusEFI_CAN_verbose.dbc \
    --base components/canmap/default_map.xml --eez components/hmi/FP_Dash.eez-project \
    --auto -o components/canmap/default_map.xml        # after UI variable changes
```

- **Generated files:** never hand-edit `components/hmi/src/ui/*`. The EEZ
  project is the source of truth. Layout changes are made in EEZ Studio, or
  with a small Python edit of the project JSON:
  - 2-space indent, `ensure_ascii=False`
  - fresh UUID `objID`s for new widgets
  - then run `tools/eez-build.sh`
- **Mapping by name:** UI variables reach the firmware by name. `vars.h` ->
  `eez_var_table.c` is a build step. Do not add per-variable C code: extend
  the map instead. Only `sys_*` variables are written by the bridge directly
  (`ui_bridge.c`, `settings_view.c`). `ui_*` variables are UI-only and never
  mapped.
- **Pages by name:** `screens.h` -> `eez_screen_table` in the same build step.
  The screen `settings` is the settings screen; every other screen is a dash
  page (`bridge/nav.c`). Do not reference `objects.<widget>` or `SCREEN_ID_*`
  from bridge code. Settings controls are two-way bound to `sys_set_*`
  variables in EEZ.
- **`sdkconfig`:** local and not committed. Defaults go in `sdkconfig.defaults`.
- **Console is UART0.** GPIO19/20 are switched from USB to CAN at boot (CH422G
  EXIO5, `board_select_can`).

## Rules

- **Producers never call LVGL.** Only `hmi/bridge` touches EEZ/LVGL, and it
  runs in lv_timers.
- **Init order** (`main.c`): settings (NVS) -> IO expander -> SD ->
  `canmap_load` -> LCD/UI -> CAN -> `canmap_start` -> datalog (initial state
  from settings). The UI bridge and the datalog columns need
  the channel table, which is fixed after `canmap_load`.
- **Map format:** a change to the format needs updates in three places:
  - `canmap.c` (parser)
  - `tools/fpdash_mapper/mapmodel.py` (writer/reader)
  - `tools/fpdash_mapper/README.md`

  Unknown elements/attributes must stay ignored (forward compatible). Bump
  `version` only for incompatible changes.
- **Owner tests:** the owner tests on hardware. Say clearly what was only built
  and not run.

## Status (2026-11-11)

Done, builds, **not run on hardware yet**:
- Restructure from the Waveshare LVGL example.
- Signal map + decoder + host tests:
  - mini_xml
  - Intel and Motorola extraction
  - format check
- fpdash_mapper GUI/CLI.
- EEZ pages `dash`, `race`, `engine` + `settings`; swipe / long-press
  navigation; last page restored.
- Settings in NVS: logging raw/params, brightness (software dimming).
- Logging: candump, MLV CSV, events.

To verify on the board first:
- the EXIO5 CAN switch
- the SD card mount (first hardware run: logging failed with "cannot create
  log dir" = card not mounted at boot; cause not known yet). Logging now stops
  on SD errors and waits for Settings > Retry SD (remount).
- whether the MLV CSV with a units row opens in MegaLogViewer
  (`~/tools/MegaLogViewerHD`)
- the arc/tile layout on the real panel, and the `race` RPM bar (the bar
  widget was made by editing the project JSON)
- swipe and long press on the GT911 (gesture thresholds), and that the
  settings checkboxes and slider write back to their variables

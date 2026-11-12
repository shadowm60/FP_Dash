# FP_Dash

A freely programmable CAN dash. It listens to the CAN bus, decodes the signals
described by a DBC file, and shows them on a UI designed in EEZ Studio. It
also logs both the decoded values and the full raw CAN trace to the SD card.

The first target ECU is **rusEFI**, using its CAN broadcast at 0x200–0x20B
(`tools/dbc/rusEFI_CAN_verbose.dbc`). Other ECUs or CAN devices only need a
DBC and a new map. The firmware does not change.

## Hardware

[Waveshare ESP32-S3-Touch-LCD-7](https://docs.waveshare.com/ESP32-S3-Touch-LCD-7):
- 800x480 RGB panel and GT911 touch
- CH422G IO expander
- TF card on SPI
- CAN transceiver

| Function | Pins |
|---|---|
| CAN TX / RX | GPIO20 / GPIO19 (shared with native USB, see below) |
| USB / CAN switch | CH422G EXIO5: low = USB, high = CAN |
| SD card | MOSI 11, SCK 12, MISO 13, CS = CH422G EXIO4 |
| I2C (CH422G, GT911) | SDA 8, SCL 9 |

**CAN and USB share GPIO19/20.** The firmware switches them to CAN at start-up,
so after boot the native USB port is gone. Use the board's **UART** USB-C port
for flashing and the serial monitor. The console is set to UART0.

CAN termination is a jumper on the board: 120 Ω is fitted by default. Remove it
if the dash is not at an end of the bus.

## How it works

```
 CAN bus ─▶ can_if ─┬─▶ canmap decoder ─▶ channels ─┬─▶ ui_bridge ─▶ EEZ variables ─▶ screen
                    │      ▲                       └─▶ datalog: par_NNN.csv
                    │      └─ map.xml (SD) or built-in map
                    └─▶ datalog: can_NNN.log (every frame), evt_NNN.txt
```

| Component | Role |
|---|---|
| `components/board` | LCD, touch, CH422G (incl. USB/CAN switch), SD card, LVGL port |
| `components/can_if` | TWAI driver: receives all IDs; optional listen-only; bus statistics |
| `components/canmap` | Loads the signal map XML and decodes frames into channels |
| `components/channels` | Latest value + timestamp of every signal; stale after the frame timeout |
| `components/datalog` | Raw candump log, MegaLogViewer CSV, event log |
| `components/settings` | Settings screen choices in NVS: logging, brightness, last page |
| `components/hmi` | EEZ Studio project + generated code (`src/ui`) + `bridge/` (signals, pages, settings) |
| `tools/fpdash_mapper` | PC tool: DBC + EEZ project -> map XML ([README](tools/fpdash_mapper/README.md)) |

### The signal map

The DBC is never parsed on the dash. `fpdash_mapper` resolves everything on the
PC and writes a flat XML that the firmware reads once at boot. The XML holds:
- bit positions
- scaling
- which signals are logged
- which signals feed which UI variables

- **SD card:** `/sdcard/fpdash/map.xml` is used when present. Change the map
  with no rebuild and no flash.
- **Built-in fallback:** `components/canmap/default_map.xml` (rusEFI, every
  signal logged).

UI variables and screens are matched **by name**. At build time
`tools/gen_eez_var_table.py` turns the EEZ-generated `vars.h` and `screens.h`
into name tables. A variable or page added in EEZ Studio is then used without
any C change.

## Changing the UI (EEZ Studio)

The UI source is `components/hmi/FP_Dash.eez-project` (EEZ Studio 0.29, LVGL
8.3, EEZ Flow). In EEZ Studio you can design the UI and preview it in the
simulator without a board.

1. Open the project in EEZ Studio. Add widgets and bind them to global
   variables:
   - a label's text = expression `oil_press_text`
   - an arc's value = `rpm`
   - hidden flag = `!cel_act`
2. New variable? Add it under *Variables* (global, type string / integer /
   float / boolean).
3. Build in EEZ Studio, or run `tools/eez-build.sh` (headless; needs a desktop
   session).
4. Map the new variable in `fpdash_mapper` and save the map to the SD card, or
   regenerate the built-in map (command in the mapper README).
5. `idf.py build flash`

Name convention:
- `<signal>` (integer/float/boolean) and `<signal>_text` (string) are mapped
  automatically.
- `ui_*` variables belong to the UI only and are never fed by the map, e.g.
  `ui_shift_rpm` (the SHIFT light threshold, default 6500). Change the default
  in EEZ Studio.
- `sys_*` variables are filled by the firmware. All are optional:

| Variable | Type | |
|---|---|---|
| `sys_can_status` | string | frames/s, bus errors, which map is used |
| `sys_log_status` | string | log files, SD errors |
| `sys_rec` | boolean | true while logging |
| `sys_set_log_raw`, `sys_set_log_params` | boolean | Settings checkboxes (two-way bound), stored |
| `sys_set_brightness` | integer | Settings slider 10–100 (two-way bound), stored |
| `sys_brightness_text` | string | "Brightness 80 %" |
| `sys_log_info`, `sys_can_info`, `sys_map_info` | string | Settings screen info texts |
| `sys_log_failed` | boolean | logging stopped by an SD error (shows *Retry SD*, action `retry_sd`) |
| `sys_version` | string | firmware / IDF version |

### Pages and navigation

Pages are found by name, at build time:
- The screen named **`settings`** is the settings screen.
- **Every other screen is a dash page**, in EEZ page order.

On a dash page:
- swipe left / right: next / previous page
- long press: settings
- *Back* on the settings screen returns to the page you came from

The last page shown is restored at power-up.

To add a page, add a screen in EEZ Studio and rebuild. It joins the swipe cycle
automatically. Labels, panels, arcs and bars on dash pages are made
non-clickable so the swipe reaches the screen. Buttons, checkboxes and sliders
keep working; a button can call the native action `open_settings`.

| Page | Content |
|---|---|
| `dash` | RPM arc with RPM and gear, CEL / LIMIT / FAN, 8 tiles (speed, coolant, oil pressure, oil temperature, MAP, lambda, battery, intake), status line |
| `race` | Full-width RPM bar, SHIFT light (`rpm >= ui_shift_rpm`), large gear, RPM, speed, lambda, ignition, TPS, coolant, oil pressure / temperature |
| `engine` | 12 tuning tiles: MAP, TPS, pedal, ignition, injector PW / duty, lambda 1 / 2, fuel trim, knock count, MAF, fuel pressure |
| `settings` | Logging on/off (raw trace, decoded signals), brightness, log / CAN / map status, firmware version |

Settings are saved to NVS 2 s after the last change. Brightness is software
dimming (a black overlay) unless `CONFIG_BACKLIGHT_PWM_GPIO` is wired; see
menuconfig.

## Logging

Logs go to `/sdcard/log/`. A session opens on the first CAN frame and closes
after `CONFIG_DATALOG_IDLE_CLOSE_MS` (10 s) without traffic, e.g. at ignition
off. All files of one session share the same number.

| File | Content |
|---|---|
| `can_NNN.log` | Every frame on the bus, candump format: `(12.345000) can0 201#E803...`. Opens in SavvyCAN, python-can, cantools. |
| `par_NNN.csv` | Every signal with `log="1"` at 20 Hz (`CONFIG_DATALOG_PARAM_PERIOD_MS`). Row 1 = names, row 2 = units. MegaLogViewer CSV; an empty cell means the value is stale. |
| `evt_NNN.txt` | Map in use (or why the SD map was rejected), CAN error counters, dropped frames. |

Timestamps are seconds since boot, because the board has no RTC.

**SD errors.** The board has no usable card detect, so the logger does not
retry on its own. After a failure (no card at boot, card full, card pulled out):
- logging stops;
- the status line says "LOG stopped: SD error";
- Settings shows the reason and a **Retry SD** button.

*Retry SD* closes any open files, unmounts and remounts the card, then logging
resumes with the next CAN frame. Insert or swap the card, then press it. A card
that mounts but has no FAT file system (e.g. exFAT) is reported as such in the
serial log. Format it as FAT32.

## Build

ESP-IDF v6.1 (EIM install):

```sh
. ~/.espressif/tools/activate_idf_v6.1.sh
idf.py build
idf.py -p /dev/ttyACM0 flash monitor     # UART USB-C port (CH343)
```

VS Code: the ESP-IDF extension setup in `.vscode/settings.json` points at the
same v6.1 install.

`sdkconfig` is local and not committed. Defaults live in `sdkconfig.defaults`.
After changing them, delete `sdkconfig` (or run `idf.py reconfigure`).

### Configuration (`idf.py menuconfig` -> FP_Dash configuration)

| Option | Default | |
|---|---|---|
| CAN bitrate | 500000 | Used when the map has no `<bus bitrate>` |
| CAN listen only | off | Leave off on a bench with only the ECU, so someone ACKs |
| Signal map file | `/sdcard/fpdash/map.xml` | |
| Default signal timeout | 1000 ms | |
| Parameter log period | 50 ms | |
| Log raw / params by default | on / on | First boot only. Afterwards the Settings screen choice is kept |
| Backlight dimming PWM GPIO | -1 | -1 = software dimming |
| Close logs after idle | 10000 ms | |

## Enclosure

`CAD/` has a parametric FreeCAD back cover with:
- a service hatch over the USB-C ports and the SD card;
- a 4-pin car connector;
- a 12 V → 5 V converter cradle;
- an optional battery bay;
- an AMPS mount.

See [CAD/README.md](CAD/README.md).

## Roadmap / ideas

- Value tables (`VAL_`) for text states (gear N/1/2..., error codes)
- Warning thresholds (coolant, oil pressure) with colour changes and a shift light
- Multiplexed signals
- Real-time clock for log timestamps

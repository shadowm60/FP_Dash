# fpdash_mapper

Maps CAN signals from a DBC file to the UI variables of the EEZ Studio project
and writes the signal map that the dash loads from the SD card.

Only the Python 3 standard library is needed (tkinter for the GUI).

## GUI

```sh
python3 tools/fpdash_mapper/fpdash_mapper.py \
    --dbc tools/dbc/rusEFI_CAN_verbose.dbc \
    --eez components/hmi/FP_Dash.eez-project \
    --map components/canmap/default_map.xml      # optional: start from an existing map
```

1. **Open DBC** loads frames and signals. If a map is already open, you can
   keep its settings for signals that are still in the new DBC.
2. **Open EEZ project** loads the UI variables (`globalVariables`).
3. **Auto-map by name** binds variables to signals with the same name:
   `rpm` -> `RPM`, `oil_press_text` -> `OilPress` (`_text` / `_str` suffix =
   string variable).
4. Select a signal to edit it:
   - include / log
   - unit, decimals
   - frame timeout
   - its UI bindings: variable, type, scale/offset, format
5. **Save map** and copy the file to the SD card as `/fpdash/map.xml`.

Variables named `sys_*` (filled by the firmware) and `ui_*` (UI-only, e.g.
thresholds) are not offered for mapping and not reported as unmapped.

The **Checks** box lists problems, such as:
- variables missing from the EEZ project
- type mismatches
- one variable fed by several signals
- EEZ variables that nothing feeds

## Command line

```sh
# regenerate the built-in map after the DBC or the UI changed, keeping the hand edits
python3 tools/fpdash_mapper/fpdash_mapper.py --dbc tools/dbc/rusEFI_CAN_verbose.dbc \
    --base components/canmap/default_map.xml --eez components/hmi/FP_Dash.eez-project \
    --auto -o components/canmap/default_map.xml

# only check a map against the UI (exit 1 on warnings)
python3 tools/fpdash_mapper/fpdash_mapper.py --map map.xml --eez components/hmi/FP_Dash.eez-project --check
```

## Map format (version 1)

```xml
<fpdash_map version="1" source="rusEFI_CAN_verbose.dbc">
  <bus bitrate="500000"/>
  <frame id="0x201" ext="0" name="BASE1" timeout_ms="1000">
    <signal name="RPM" start="0" len="16" order="intel" type="unsigned"
            factor="1" offset="0" unit="RPM" decimals="0" log="1">
      <ui var="rpm" type="integer"/>
      <ui var="rpm_text" type="string" format="%.0f"/>
    </signal>
  </frame>
</fpdash_map>
```

| Element / attribute | Meaning |
|---|---|
| `bus bitrate` | CAN bitrate. Overrides the Kconfig default. |
| `frame id` | CAN ID, decimal or `0x..`. `ext="1"` = 29-bit. |
| `frame timeout_ms` | A signal not refreshed for this long is shown as `--` and logged as an empty cell. Default `CONFIG_CANMAP_DEFAULT_TIMEOUT_MS`. A signal can override it. |
| `signal name` | Channel name: the column in the parameter log. Must be unique in the map. |
| `start`, `len` | DBC bit position and length. For `motorola`, `start` is the MSB, as in the DBC. |
| `order` | `intel` (DBC `@1`) or `motorola` (DBC `@0`). |
| `type` | `unsigned`, `signed` or `float` (32-bit IEEE, DBC `SIG_VALTYPE_ 1`). |
| `factor`, `offset` | value = raw x factor + offset. |
| `unit`, `decimals` | Log header and number formatting. |
| `log` | `1` = column in the parameter CSV. |
| `ui var` | EEZ global variable name, as written in EEZ Studio. |
| `ui type` | `integer`, `float`, `string` or `boolean`. Must match the variable type in EEZ. |
| `ui scale`, `ui offset` | Unit conversion for the display only: shown = value x scale + offset. |
| `ui format` | String variables only: printf format with one float conversion. Default `%.<decimals>f`. |

When a signal times out, the firmware writes `--` to string variables, 0 to
integer and float variables, and false to booleans.

Unknown elements and attributes are ignored, so newer maps still load on older
firmware. If the SD map has an error, the dash falls back to the built-in map:
- the status line says "SD map error!"
- the reason (line number + message) goes to the serial log and to `evt_NNN.txt`

Not supported yet:
- multiplexed signals (the tool leaves them out)
- `VAL_` value tables (e.g. gear 0 -> "N")

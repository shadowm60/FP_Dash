/*
 * canmap - the signal map: which CAN frames carry which signals, and which
 * UI variables show them.
 *
 * The map is an XML file written by tools/fpdash_mapper from a DBC and the
 * EEZ Studio project. Everything is resolved on the PC (bit positions,
 * scaling, UI variable names and types), so the firmware only reads a flat
 * table once at boot. Format (see tools/fpdash_mapper/README.md):
 *
 *   <fpdash_map version="1">
 *     <bus bitrate="500000"/>
 *     <frame id="0x201" ext="0" name="BASE1" timeout_ms="1000">
 *       <signal name="RPM" start="0" len="16" order="intel" type="unsigned"
 *               factor="1" offset="0" unit="RPM" decimals="0" log="1">
 *         <ui var="rpm" type="integer"/>
 *         <ui var="rpm_text" type="string" format="%.0f"/>
 *       </signal>
 *     </frame>
 *   </fpdash_map>
 *
 * Load order: CONFIG_CANMAP_SD_PATH on the SD card, else the built-in map
 * (default_map.xml, rusEFI broadcast). Every <signal> becomes a channel
 * (channels.h). canmap_start() then decodes received frames into channels.
 */
#ifndef CANMAP_H
#define CANMAP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "esp_err.h"
#include "channels.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CANMAP_VAR_NAME_MAX   32
#define CANMAP_FORMAT_MAX     16

typedef enum {
    CANMAP_UI_INTEGER = 0,
    CANMAP_UI_FLOAT,
    CANMAP_UI_STRING,
    CANMAP_UI_BOOLEAN,
} canmap_ui_type_t;

/* One <ui> element: a channel shown in an EEZ variable. */
typedef struct {
    ch_id_t          ch;
    char             var[CANMAP_VAR_NAME_MAX];   /* EEZ global variable name */
    canmap_ui_type_t type;
    float            scale;     /* shown = value * scale + offset (unit conversion) */
    float            offset;
    char             format[CANMAP_FORMAT_MAX];  /* string vars: printf format for one float */
    float            step;      /* > 0: shown value rounded to a multiple of step (e.g. 50 rpm) */
    uint16_t         period_ms; /* > 0: update at most this often; 0 = bridge default for the type */
} canmap_ui_binding_t;

typedef struct {
    bool     from_sd;           /* false: built-in map */
    char     source[48];        /* map file path, or "built-in" */
    char     description[48];   /* <fpdash_map source="..."> e.g. the DBC name */
    uint32_t bitrate;           /* 0 = not set in the map */
    uint16_t frames;
    uint16_t signals;
    uint16_t ui_bindings;
    char     error[64];         /* why the SD map was rejected, "" if none */
} canmap_info_t;

typedef struct {
    uint32_t frames_decoded;    /* frames with an entry in the map */
    uint32_t frames_unknown;    /* frames not in the map (still logged raw) */
    uint32_t frames_dropped;    /* decoder queue full */
} canmap_stats_t;

/* Builds the channel table. Call once, before canmap_start() and before any
 * consumer reads channels. Falls back to the built-in map on any SD error. */
esp_err_t canmap_load(void);

/* Subscribes to can_if and starts the decoder task. */
esp_err_t canmap_start(void);

void canmap_get_info(canmap_info_t *out);
void canmap_get_stats(canmap_stats_t *out);

/* UI bindings of the loaded map (stable after canmap_load). */
const canmap_ui_binding_t *canmap_ui_bindings(size_t *count);

#ifdef __cplusplus
}
#endif

#endif /* CANMAP_H */

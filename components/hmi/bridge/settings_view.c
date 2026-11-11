/*
 * settings_view - the settings screen, by variable name like everything else:
 *
 *   sys_set_log_raw      boolean  checkbox  -> datalog raw trace     (stored)
 *   sys_set_log_params   boolean  checkbox  -> datalog signals CSV   (stored)
 *   sys_set_brightness   integer  slider    -> brightness 10..100 %  (stored)
 *   sys_brightness_text  string   "Brightness 80 %"
 *   sys_log_info         string   log files, rows, dropped frames, errors
 *   sys_log_failed       boolean  logging stopped by an SD error: shows Retry SD
 *   sys_can_info         string   bitrate, bus state, frame counters
 *   sys_map_info         string   map in use, counts, why the SD map was rejected
 *   sys_version          string   firmware version
 *
 * The controls are two-way bound in EEZ (checked state / value = variable),
 * so the firmware only reads and writes the variables. All are optional.
 * Stored values are pushed to the variables at start; then the variables are
 * polled and every change is applied and saved.
 */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "esp_app_desc.h"
#include "sdkconfig.h"
#include "eez_vars.h"
#include "eez_var_table.h"
#include "settings.h"
#include "datalog.h"
#include "canmap.h"
#include "can_if.h"
#include "views.h"

typedef enum {
    V_LOG_RAW, V_LOG_PARAMS, V_BRIGHTNESS, V_BRIGHTNESS_TEXT,
    V_LOG_INFO, V_LOG_FAILED, V_CAN_INFO, V_MAP_INFO, V_VERSION, V_COUNT
} var_t;

static const char *const s_names[V_COUNT] = {
    "sys_set_log_raw", "sys_set_log_params", "sys_set_brightness", "sys_brightness_text",
    "sys_log_info", "sys_log_failed", "sys_can_info", "sys_map_info", "sys_version",
};
static int32_t s_var[V_COUNT];

static bool    s_log_raw, s_log_params;
static int32_t s_brightness;

static void set_str(var_t v, const char *text)
{
    if (s_var[v] >= 0) {
        eez_var_set_string((uint32_t)s_var[v], text);
    }
}

static void brightness_text(void)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "Brightness %" PRId32 " %%", s_brightness);
    set_str(V_BRIGHTNESS_TEXT, buf);
}

void settings_view_init(void)
{
    for (int i = 0; i < V_COUNT; i++) {
        s_var[i] = eez_var_lookup(s_names[i]);
    }

    app_settings_t cfg;
    settings_get(&cfg);
    s_log_raw = cfg.log_raw;
    s_log_params = cfg.log_params;
    s_brightness = cfg.brightness;
    brightness_init(cfg.brightness);

    if (s_var[V_LOG_RAW] >= 0) {
        eez_var_set_bool((uint32_t)s_var[V_LOG_RAW], s_log_raw);
    }
    if (s_var[V_LOG_PARAMS] >= 0) {
        eez_var_set_bool((uint32_t)s_var[V_LOG_PARAMS], s_log_params);
    }
    if (s_var[V_BRIGHTNESS] >= 0) {
        eez_var_set_int((uint32_t)s_var[V_BRIGHTNESS], s_brightness);
    }
    brightness_text();

    char buf[96];
    snprintf(buf, sizeof(buf), "FP_Dash %s  (IDF %s)", esp_app_get_description()->version,
             esp_app_get_description()->idf_ver);
    set_str(V_VERSION, buf);
}

void settings_view_refresh(void)
{
    if (s_var[V_LOG_RAW] >= 0) {
        bool on = eez_var_get_bool((uint32_t)s_var[V_LOG_RAW]);
        if (on != s_log_raw) {
            s_log_raw = on;
            settings_set_log_raw(on);
            datalog_enable_raw(on);
        }
    }
    if (s_var[V_LOG_PARAMS] >= 0) {
        bool on = eez_var_get_bool((uint32_t)s_var[V_LOG_PARAMS]);
        if (on != s_log_params) {
            s_log_params = on;
            settings_set_log_params(on);
            datalog_enable_params(on);
        }
    }
    if (s_var[V_BRIGHTNESS] >= 0) {
        int32_t b = eez_var_get_int((uint32_t)s_var[V_BRIGHTNESS]);
        b = b < 10 ? 10 : (b > 100 ? 100 : b);
        if (b != s_brightness) {
            s_brightness = b;
            brightness_set((uint8_t)b);
            settings_set_brightness((uint8_t)b);
            brightness_text();
        }
    }
}

static const char *bus_state(can_if_bus_state_t st)
{
    switch (st) {
        case CAN_IF_BUS_ACTIVE:  return "active";
        case CAN_IF_BUS_WARNING: return "warning";
        case CAN_IF_BUS_PASSIVE: return "error passive";
        case CAN_IF_BUS_OFF:     return "BUS OFF";
        default:                 return "stopped";
    }
}

static void log_info(void)
{
    datalog_status_t ls;
    datalog_get_status(&ls);
    char buf[256];
    int n = 0;

    if (ls.retrying) {
        n += snprintf(buf + n, sizeof(buf) - n, "Mounting SD card...\n");
    } else if (ls.failed) {
        n += snprintf(buf + n, sizeof(buf) - n, "Logging stopped: %s\nInsert / check the card, then Retry SD", ls.error);
    } else if (!ls.raw_enabled && !ls.params_enabled) {
        n += snprintf(buf + n, sizeof(buf) - n, "Logging off");
    } else if (ls.raw_active || ls.params_active) {
        if (ls.raw_active) {
            n += snprintf(buf + n, sizeof(buf) - n, "%s  %" PRIu32 " frames\n", ls.raw_file, ls.raw_frames);
        }
        if (ls.params_active) {
            n += snprintf(buf + n, sizeof(buf) - n, "%s  %" PRIu32 " rows\n", ls.params_file, ls.params_rows);
        }
    } else {
        n += snprintf(buf + n, sizeof(buf) - n, "Waiting for CAN traffic\n");
    }
    if (ls.raw_dropped && n < (int)sizeof(buf)) {
        n += snprintf(buf + n, sizeof(buf) - n, "Dropped: %" PRIu32 " frames\n", ls.raw_dropped);
    }
    if (!ls.failed && ls.error[0] && n < (int)sizeof(buf)) {
        snprintf(buf + n, sizeof(buf) - n, "Last error: %s", ls.error);
    }
    set_str(V_LOG_INFO, buf);
    if (s_var[V_LOG_FAILED] >= 0) {
        eez_var_set_bool((uint32_t)s_var[V_LOG_FAILED], ls.failed && !ls.retrying);
    }
}

static void can_info(void)
{
    can_if_stats_t cs;
    can_if_get_stats(&cs);
    canmap_info_t mi;
    canmap_get_info(&mi);
    canmap_stats_t ms;
    canmap_get_stats(&ms);
    uint32_t bitrate = mi.bitrate ? mi.bitrate : CONFIG_CAN_BITRATE;

    char buf[256];
    snprintf(buf, sizeof(buf),
             "%" PRIu32 " kbit/s%s, bus %s\n"
             "%" PRIu32 " frames/s, %" PRIu32 " received\n"
             "Decoded %" PRIu32 ", not in map %" PRIu32 "\n"
             "Bus errors %" PRIu32 ", dropped %" PRIu32,
             bitrate / 1000, CAN_IF_LISTEN_ONLY_DEFAULT ? " listen only" : "", bus_state(cs.state),
             bridge_can_rate(), cs.rx_frames,
             ms.frames_decoded, ms.frames_unknown,
             cs.bus_errors, cs.rx_dropped + ms.frames_dropped);
    set_str(V_CAN_INFO, buf);
}

static void map_info(void)
{
    canmap_info_t mi;
    canmap_get_info(&mi);
    char buf[256];
    int n = snprintf(buf, sizeof(buf), "%s\n%s\n%u frames, %u signals, %u on screen",
                     mi.from_sd ? mi.source : "Built-in map",
                     mi.description[0] ? mi.description : "-",
                     mi.frames, mi.signals, mi.ui_bindings);
    if (mi.error[0] && n < (int)sizeof(buf)) {
        snprintf(buf + n, sizeof(buf) - n, "\nSD map rejected: %s", mi.error);
    }
    set_str(V_MAP_INFO, buf);
}

void settings_view_refresh_info(void)
{
    log_info();
    can_info();
    map_info();
}

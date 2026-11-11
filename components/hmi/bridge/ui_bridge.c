/*
 * ui_bridge - the only code that writes EEZ variables.
 *
 * Runs as lv_timers inside the LVGL task: reads channels (filled by the CAN
 * decoder) and pushes changed values into EEZ Flow global variables. Which
 * channel goes to which variable comes from the signal map (<ui> elements),
 * resolved by name at start-up, so new UI variables need no C changes:
 * add the variable in EEZ Studio, rebuild the UI, map it in fpdash_mapper.
 *
 * Variables named sys_* belong to the firmware (status line):
 *   sys_can_status  string   bus state, frames/s, map in use
 *   sys_log_status  string   log files / SD state
 *   sys_rec         boolean  a log file is being written
 * They are optional: a UI without them works too. The settings screen
 * variables (sys_set_*, sys_*_info) are handled by settings_view.c, page
 * navigation by nav.c.
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <inttypes.h>
#include "lvgl.h"
#include "esp_log.h"
#include "ui.h"
#include "hmi.h"
#include "eez_vars.h"
#include "eez_var_table.h"
#include "canmap.h"
#include "channels.h"
#include "can_if.h"
#include "datalog.h"
#include "settings.h"
#include "views.h"

static const char *TAG = "ui_bridge";

#define SIGNAL_PERIOD_MS    40      /* 25 Hz, the panel refreshes at ~40 Hz */
#define STATUS_PERIOD_MS    500
#define SETTINGS_PERIOD_MS  100     /* settings controls: slider feels immediate */
#define STALE_TEXT          "--"

typedef struct {
    const canmap_ui_binding_t *b;
    uint32_t var;
    bool     sent;              /* something was written at least once */
    bool     last_valid;
    float    last_num;
    char     last_str[24];
} binding_rt_t;

static binding_rt_t *s_rt;
static size_t        s_nrt;
static int32_t       s_var_can = -1, s_var_log = -1, s_var_rec = -1;

/* ---------------------------------------------------------------- var table */

bool eez_name_equal(const char *a, const char *b)
{
    while (*a || *b) {
        if (*a == '_') { a++; continue; }
        if (*b == '_') { b++; continue; }
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return false;
        }
        a++;
        b++;
    }
    return true;
}

int32_t eez_var_lookup(const char *name)
{
    for (unsigned i = 0; i < eez_var_table_len; i++) {
        if (eez_name_equal(eez_var_table[i].name, name)) {
            return (int32_t)eez_var_table[i].index;
        }
    }
    return -1;
}

/* ---------------------------------------------------------------- signals */

static void push(binding_rt_t *r, bool valid, float v)
{
    const canmap_ui_binding_t *b = r->b;
    switch (b->type) {
    case CANMAP_UI_INTEGER: {
        float iv = valid ? roundf(v) : 0.0f;
        if (!r->sent || iv != r->last_num) {
            eez_var_set_int(r->var, (int32_t)iv);
            r->last_num = iv;
        }
        break;
    }
    case CANMAP_UI_FLOAT: {
        float fv = valid ? v : 0.0f;
        if (!r->sent || fv != r->last_num) {
            eez_var_set_float(r->var, fv);
            r->last_num = fv;
        }
        break;
    }
    case CANMAP_UI_BOOLEAN: {
        float bv = (valid && v != 0.0f) ? 1.0f : 0.0f;
        if (!r->sent || bv != r->last_num) {
            eez_var_set_bool(r->var, bv != 0.0f);
            r->last_num = bv;
        }
        break;
    }
    case CANMAP_UI_STRING: {
        char buf[sizeof(r->last_str)];
        if (valid) {
            snprintf(buf, sizeof(buf), b->format, (double)v);
        } else {
            snprintf(buf, sizeof(buf), "%s", STALE_TEXT);
        }
        if (!r->sent || strcmp(buf, r->last_str) != 0) {
            eez_var_set_string(r->var, buf);
            strcpy(r->last_str, buf);
        }
        break;
    }
    }
    r->sent = true;
    r->last_valid = valid;
}

static void signal_timer_cb(lv_timer_t *t)
{
    uint32_t now = can_if_now_ms();
    for (size_t i = 0; i < s_nrt; i++) {
        binding_rt_t *r = &s_rt[i];
        ch_sample_t smp;
        bool valid = ch_get(r->b->ch, now, &smp);
        push(r, valid, valid ? smp.value * r->b->scale + r->b->offset : 0.0f);
    }
}

/* ---------------------------------------------------------------- status line */

static uint32_t s_prev_rx, s_prev_ms, s_rate;

uint32_t bridge_can_rate(void)
{
    return s_rate;
}

static const char *bus_state_text(can_if_bus_state_t st)
{
    switch (st) {
        case CAN_IF_BUS_WARNING: return "warning";
        case CAN_IF_BUS_PASSIVE: return "error passive";
        case CAN_IF_BUS_OFF:     return "BUS OFF";
        case CAN_IF_BUS_STOPPED: return "stopped";
        default:                 return NULL;
    }
}

static void status_timer_cb(lv_timer_t *t)
{
    uint32_t now = can_if_now_ms();
    char buf[96];

    can_if_stats_t cs;
    can_if_get_stats(&cs);
    canmap_info_t mi;
    canmap_get_info(&mi);
    uint32_t dt = now - s_prev_ms;
    uint32_t rate = (dt > 0 && s_prev_ms != 0) ? (cs.rx_frames - s_prev_rx) * 1000u / dt : 0;
    s_prev_rx = cs.rx_frames;
    s_prev_ms = now;
    s_rate = rate;

    if (s_var_can >= 0) {
        const char *st = bus_state_text(cs.state);
        const char *map = mi.from_sd ? "SD map" : (mi.error[0] ? "built-in map (SD map error!)" : "built-in map");
        if (st != NULL) {
            snprintf(buf, sizeof(buf), "CAN %s  -  %s", st, map);
        } else if (rate == 0) {
            snprintf(buf, sizeof(buf), "CAN: no data  -  %s", map);
        } else {
            snprintf(buf, sizeof(buf), "CAN %" PRIu32 " fr/s  -  %s", rate, map);
        }
        eez_var_set_string((uint32_t)s_var_can, buf);
    }

    datalog_status_t ls;
    datalog_get_status(&ls);
    if (s_var_log >= 0) {
        if (ls.failed) {
            snprintf(buf, sizeof(buf), "LOG stopped: SD error (Settings > Retry SD)");
        } else if (!ls.raw_enabled && !ls.params_enabled) {
            snprintf(buf, sizeof(buf), "LOG off");
        } else if (ls.raw_active || ls.params_active) {
            int n = snprintf(buf, sizeof(buf), "%s", ls.raw_active ? ls.raw_file : "");
            if (ls.params_active) {
                snprintf(buf + n, sizeof(buf) - n, "%s%s", n ? "  " : "", ls.params_file);
            }
        } else if (ls.error[0] != '\0') {
            snprintf(buf, sizeof(buf), "LOG: %s", ls.error);
        } else {
            snprintf(buf, sizeof(buf), "LOG: waiting for CAN");
        }
        eez_var_set_string((uint32_t)s_var_log, buf);
    }
    if (s_var_rec >= 0) {
        eez_var_set_bool((uint32_t)s_var_rec, ls.raw_active || ls.params_active);
    }
    if (nav_settings_visible()) {
        settings_view_refresh_info();
    }
}

static void settings_timer_cb(lv_timer_t *t)
{
    settings_view_refresh();
}

/* ---------------------------------------------------------------- init */

static void resolve_bindings(void)
{
    size_t n;
    const canmap_ui_binding_t *b = canmap_ui_bindings(&n);
    s_rt = lv_mem_alloc(sizeof(*s_rt) * (n ? n : 1));
    if (s_rt == NULL) {
        ESP_LOGE(TAG, "out of memory");
        return;
    }
    s_nrt = 0;
    for (size_t i = 0; i < n; i++) {
        int32_t idx = eez_var_lookup(b[i].var);
        if (idx < 0) {
            ESP_LOGW(TAG, "map binds '%s', which is not a variable of this UI: ignored", b[i].var);
            continue;
        }
        if (b[i].ch == CH_NONE) {
            continue;
        }
        memset(&s_rt[s_nrt], 0, sizeof(s_rt[0]));
        s_rt[s_nrt].b = &b[i];
        s_rt[s_nrt].var = (uint32_t)idx;
        s_nrt++;
    }
    s_var_can = eez_var_lookup("sys_can_status");
    s_var_log = eez_var_lookup("sys_log_status");
    s_var_rec = eez_var_lookup("sys_rec");
    ESP_LOGI(TAG, "%u of %u map bindings resolved", (unsigned)s_nrt, (unsigned)n);
}

void hmi_init(void)
{
    ui_init();
    resolve_bindings();
    settings_view_init();

    app_settings_t cfg;
    settings_get(&cfg);
    nav_init(cfg.page);

    lv_timer_create(signal_timer_cb, SIGNAL_PERIOD_MS, NULL);
    lv_timer_create(status_timer_cb, STATUS_PERIOD_MS, NULL);
    lv_timer_create(settings_timer_cb, SETTINGS_PERIOD_MS, NULL);
}

void hmi_tick(void)
{
    ui_tick();
}

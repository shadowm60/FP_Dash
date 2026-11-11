/*
 * nav - page navigation, driven by the screens of the EEZ project:
 *
 *   - the screen named "settings" is the settings screen
 *   - every other screen is a dash page, in EEZ page order
 *
 * A page added in EEZ Studio is therefore part of the swipe cycle without any
 * C change. On a dash page: swipe left / right = next / previous page, long
 * press = settings. The settings screen closes with the close_settings action
 * (Back button). Buttons on a page may also call open_settings.
 *
 * Touches must reach the screen object for the gestures, so everything on a
 * dash page except real controls (buttons, checkboxes, sliders...) is made
 * non-clickable. The page shown last is stored and restored at power-up.
 */
#include <string.h>
#include "lvgl.h"
#include "esp_log.h"
#include "ui.h"
#include "screens.h"
#include "eez_var_table.h"
#include "settings.h"
#include "views.h"

static const char *TAG = "nav";

#define NAV_ANIM_MS     200
#define MAX_PAGES       16

typedef struct {
    int16_t     id;             /* EEZ screen id (1-based) */
    const char *name;           /* as in eez_screen_table, e.g. "DASH" */
} page_t;

static page_t  s_pages[MAX_PAGES];
static int     s_npages;
static int     s_cur;                   /* index of the current / last dash page */
static int16_t s_settings_id = -1;

/* EEZ puts the screens first in `objects`, in screen id order. */
static lv_obj_t *screen_obj(int16_t id)
{
    if (id < _SCREEN_ID_FIRST || id > _SCREEN_ID_LAST) {
        return NULL;
    }
    return ((lv_obj_t **)&objects)[id - 1];
}

static int page_index(int16_t id)
{
    for (int i = 0; i < s_npages; i++) {
        if (s_pages[i].id == id) {
            return i;
        }
    }
    return -1;
}

static void show_page(int idx, lv_scr_load_anim_t anim)
{
    s_cur = idx;
    eez_flow_set_screen(s_pages[idx].id, anim, NAV_ANIM_MS, 0);
    settings_set_page(s_pages[idx].name);
}

static void step(int dir)
{
    int cur = page_index(eez_flow_get_current_screen());
    if (cur < 0 || s_npages < 2) {
        return;
    }
    show_page((cur + dir + s_npages) % s_npages,
              dir > 0 ? LV_SCR_LOAD_ANIM_MOVE_LEFT : LV_SCR_LOAD_ANIM_MOVE_RIGHT);
}

static void page_event_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_GESTURE) {
        lv_indev_t *indev = lv_indev_get_act();
        lv_dir_t dir = indev ? lv_indev_get_gesture_dir(indev) : LV_DIR_NONE;
        if (dir == LV_DIR_LEFT || dir == LV_DIR_RIGHT) {
            lv_indev_wait_release(indev);
            step(dir == LV_DIR_LEFT ? +1 : -1);
        }
        return;
    }
    nav_open_settings();                /* LV_EVENT_LONG_PRESSED */
}

static bool is_control(lv_obj_t *o)
{
    return lv_obj_check_type(o, &lv_btn_class) || lv_obj_check_type(o, &lv_checkbox_class) ||
           lv_obj_check_type(o, &lv_slider_class) || lv_obj_check_type(o, &lv_switch_class) ||
           lv_obj_check_type(o, &lv_dropdown_class) || lv_obj_check_type(o, &lv_textarea_class) ||
           lv_obj_check_type(o, &lv_roller_class);
}

static void make_passive(lv_obj_t *o)
{
    for (uint32_t i = 0; i < lv_obj_get_child_cnt(o); i++) {
        lv_obj_t *c = lv_obj_get_child(o, i);
        if (is_control(c)) {
            continue;                   /* gestures still bubble up from controls */
        }
        lv_obj_clear_flag(c, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
        make_passive(c);
    }
}

void nav_init(const char *page)
{
    for (unsigned i = 0; i < eez_screen_table_len; i++) {
        const eez_var_entry_t *s = &eez_screen_table[i];
        if (eez_name_equal(s->name, "settings")) {
            s_settings_id = (int16_t)s->index;
            continue;
        }
        lv_obj_t *scr = screen_obj((int16_t)s->index);
        if (scr == NULL) {
            ESP_LOGW(TAG, "page '%s' not created at start (EEZ: 'Create at start' off?): not in the swipe cycle",
                     s->name);
            continue;
        }
        if (s_npages == MAX_PAGES) {
            ESP_LOGW(TAG, "more than %d pages, '%s' and later ignored", MAX_PAGES, s->name);
            break;
        }
        make_passive(scr);
        lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);     /* scrolling would eat the swipe */
        lv_obj_add_event_cb(scr, page_event_cb, LV_EVENT_GESTURE, NULL);
        lv_obj_add_event_cb(scr, page_event_cb, LV_EVENT_LONG_PRESSED, NULL);
        s_pages[s_npages].id = (int16_t)s->index;
        s_pages[s_npages].name = s->name;
        s_npages++;
    }
    ESP_LOGI(TAG, "%d dash page(s), settings screen %s", s_npages, s_settings_id > 0 ? "found" : "missing");

    s_cur = 0;
    for (int i = 0; i < s_npages; i++) {
        if (page && eez_name_equal(s_pages[i].name, page)) {
            s_cur = i;
        }
    }
    if (s_npages > 0 && eez_flow_get_current_screen() != s_pages[s_cur].id) {
        eez_flow_set_screen(s_pages[s_cur].id, LV_SCR_LOAD_ANIM_NONE, 0, 0);
    }
}

bool nav_settings_visible(void)
{
    return s_settings_id > 0 && eez_flow_get_current_screen() == s_settings_id;
}

void nav_open_settings(void)
{
    if (s_settings_id < 0 || nav_settings_visible()) {
        return;
    }
    int cur = page_index(eez_flow_get_current_screen());
    if (cur >= 0) {
        s_cur = cur;                    /* return here */
    }
    settings_view_refresh_info();       /* no stale texts during the animation */
    eez_flow_set_screen(s_settings_id, LV_SCR_LOAD_ANIM_FADE_IN, NAV_ANIM_MS, 0);
}

void nav_close_settings(void)
{
    if (s_npages > 0) {
        show_page(s_cur, LV_SCR_LOAD_ANIM_FADE_IN);
    }
}

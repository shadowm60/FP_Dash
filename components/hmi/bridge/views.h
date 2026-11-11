/* Internal to hmi/bridge: parts of the bridge, all run in the LVGL task. */
#ifndef HMI_VIEWS_H
#define HMI_VIEWS_H

#include <stdint.h>
#include <stdbool.h>

/* nav.c - dash pages and the settings screen, found by name (eez_screen_table).
 * Called once after ui_init(); `page` = stored page name ("" = first page). */
void nav_init(const char *page);
void nav_open_settings(void);
void nav_close_settings(void);
bool nav_settings_visible(void);

/* settings_view.c - sys_set_* variables <-> settings / datalog / brightness,
 * sys_*_info texts. */
void settings_view_init(void);
void settings_view_refresh(void);       /* fast: polls the controls */
void settings_view_refresh_info(void);  /* slow: info texts */

/* ui_bridge.c - received CAN frames per second (status timer). */
uint32_t bridge_can_rate(void);

/* brightness.c */
void brightness_init(uint8_t percent);
void brightness_set(uint8_t percent);

#endif

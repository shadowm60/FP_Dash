#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_calib_handler(lv_event_t * e);
extern void action_calib_handle_top_left(lv_event_t * e);
extern void action_calib_handle_top_right(lv_event_t * e);
extern void action_calib_handler_bottom_left(lv_event_t * e);
extern void action_calib_handle_bottom_right(lv_event_t * e);
extern void action_calib_handler_center(lv_event_t * e);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/
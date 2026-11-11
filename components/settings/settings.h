/*
 * settings - persistent user configuration (NVS).
 *
 * All values live in one versioned blob. Setters update RAM immediately and
 * schedule a save SAVE_DELAY_MS later, so dragging a slider causes a single
 * flash write. Thread safe.
 */
#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SETTINGS_PAGE_LEN   24

typedef struct {
    bool     log_raw;                   /* raw CAN trace (candump) */
    bool     log_params;                /* decoded signals (MegaLogViewer CSV) */
    uint8_t  brightness;                /* display brightness, 10..100 % */
    char     page[SETTINGS_PAGE_LEN];   /* EEZ name of the last dash page shown, "" = first */
    /* Fields added later go at the end: a shorter stored blob of the same
     * version is loaded and the new fields keep their defaults. */
} app_settings_t;

/* Initialises NVS (erasing it if the partition is corrupt/outdated) and loads
 * the stored settings. Defaults: logging from CONFIG_DATALOG_*_AT_BOOT,
 * brightness 100 %, first page. */
esp_err_t settings_init(void);

void settings_get(app_settings_t *out);

void settings_set_log_raw(bool on);
void settings_set_log_params(bool on);
void settings_set_brightness(uint8_t percent);
void settings_set_page(const char *name);

/* Write pending changes now (e.g. before restart). */
esp_err_t settings_flush(void);

#ifdef __cplusplus
}
#endif

#endif /* SETTINGS_H */

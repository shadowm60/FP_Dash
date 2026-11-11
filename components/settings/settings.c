#include "settings.h"

#include <stddef.h>
#include <string.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "freertos/FreeRTOS.h"
#include "sdkconfig.h"

static const char *TAG = "settings";

#define NVS_NAMESPACE   "fp_dash"
#define NVS_KEY         "cfg"
#define SETTINGS_VER    1
#define SAVE_DELAY_MS   2000

typedef struct {
    uint16_t       version;
    uint16_t       size;
    app_settings_t data;
} stored_t;

static app_settings_t     s_cfg;
static bool               s_dirty;
static portMUX_TYPE       s_lock = portMUX_INITIALIZER_UNLOCKED;
static esp_timer_handle_t s_save_timer;

static void set_defaults(app_settings_t *c)
{
    memset(c, 0, sizeof(*c));
#ifdef CONFIG_DATALOG_RAW_AT_BOOT
    c->log_raw = true;
#endif
#ifdef CONFIG_DATALOG_PARAMS_AT_BOOT
    c->log_params = true;
#endif
    c->brightness = 100;
}

esp_err_t settings_flush(void)
{
    stored_t blob = { .version = SETTINGS_VER, .size = sizeof(app_settings_t) };

    portENTER_CRITICAL(&s_lock);
    bool dirty = s_dirty;
    blob.data = s_cfg;
    s_dirty = false;
    portEXIT_CRITICAL(&s_lock);

    if (!dirty) {
        return ESP_OK;
    }

    nvs_handle_t h;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &h);
    if (err == ESP_OK) {
        err = nvs_set_blob(h, NVS_KEY, &blob, sizeof(blob));
        if (err == ESP_OK) {
            err = nvs_commit(h);
        }
        nvs_close(h);
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "save failed: %s", esp_err_to_name(err));
        portENTER_CRITICAL(&s_lock);
        s_dirty = true;     /* retry on next change */
        portEXIT_CRITICAL(&s_lock);
    } else {
        ESP_LOGI(TAG, "saved");
    }
    return err;
}

static void save_timer_cb(void *arg)
{
    settings_flush();
}

static void mark_dirty(void)
{
    /* called with s_lock released */
    if (s_save_timer) {
        esp_timer_stop(s_save_timer);
        esp_timer_start_once(s_save_timer, SAVE_DELAY_MS * 1000ULL);
    }
}

static esp_err_t load(void)
{
    nvs_handle_t h;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &h);
    if (err != ESP_OK) {
        return err;
    }
    stored_t blob;
    size_t len = sizeof(blob);
    err = nvs_get_blob(h, NVS_KEY, &blob, &len);
    nvs_close(h);
    if (err != ESP_OK) {
        return err;
    }
    /* Accept blobs written before fields were appended (same version, shorter). */
    size_t hdr = offsetof(stored_t, data);
    if (len < hdr || blob.version != SETTINGS_VER || blob.size > sizeof(app_settings_t) ||
        len != hdr + blob.size) {
        ESP_LOGW(TAG, "stored settings incompatible (ver %u), using defaults", blob.version);
        return ESP_ERR_INVALID_VERSION;
    }
    memcpy(&s_cfg, &blob.data, blob.size);      /* s_cfg holds defaults for the rest */
    if (s_cfg.brightness < 10 || s_cfg.brightness > 100) {
        s_cfg.brightness = 100;
    }
    s_cfg.page[sizeof(s_cfg.page) - 1] = '\0';
    return ESP_OK;
}

esp_err_t settings_init(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition erased (%s)", esp_err_to_name(err));
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    set_defaults(&s_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_flash_init: %s", esp_err_to_name(err));
        return err;
    }

    if (load() == ESP_OK) {
        ESP_LOGI(TAG, "loaded (log raw %d params %d, brightness %u, page '%s')",
                 s_cfg.log_raw, s_cfg.log_params, s_cfg.brightness, s_cfg.page);
    } else {
        ESP_LOGI(TAG, "no stored settings, using defaults");
    }

    const esp_timer_create_args_t args = { .callback = save_timer_cb, .name = "settings_save" };
    return esp_timer_create(&args, &s_save_timer);
}

void settings_get(app_settings_t *out)
{
    portENTER_CRITICAL(&s_lock);
    *out = s_cfg;
    portEXIT_CRITICAL(&s_lock);
}

/* Assign a field and schedule a save only if the value actually changed. */
#define SETTINGS_SETTER(field, value)               \
    do {                                            \
        bool changed = false;                       \
        portENTER_CRITICAL(&s_lock);                \
        if (s_cfg.field != (value)) {               \
            s_cfg.field = (value);                  \
            s_dirty = changed = true;               \
        }                                           \
        portEXIT_CRITICAL(&s_lock);                 \
        if (changed) {                              \
            mark_dirty();                           \
        }                                           \
    } while (0)

void settings_set_log_raw(bool on)              { SETTINGS_SETTER(log_raw, on); }
void settings_set_log_params(bool on)           { SETTINGS_SETTER(log_params, on); }
void settings_set_brightness(uint8_t percent)   { SETTINGS_SETTER(brightness, percent); }

void settings_set_page(const char *name)
{
    char buf[SETTINGS_PAGE_LEN] = { 0 };
    strncpy(buf, name ? name : "", sizeof(buf) - 1);
    bool changed = false;
    portENTER_CRITICAL(&s_lock);
    if (strcmp(s_cfg.page, buf) != 0) {
        memcpy(s_cfg.page, buf, sizeof(buf));
        s_dirty = changed = true;
    }
    portEXIT_CRITICAL(&s_lock);
    if (changed) {
        mark_dirty();
    }
}

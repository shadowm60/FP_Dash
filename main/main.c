/*
 * FP_Dash - freely programmable CAN dash. Only wiring and init order live here.
 *
 *   settings  user choices from the settings screen (NVS)
 *   board     LCD, touch, CH422G IO expander, SD card, LVGL port
 *   channels  latest value of every decoded signal
 *   canmap    signal map (SD card XML or built-in rusEFI map) + CAN decoder
 *   can_if    TWAI driver, RX dispatch, bus statistics
 *   datalog   SD card logging: raw CAN trace, decoded signals, events
 *   hmi       EEZ Studio UI + bridge (runs inside the LVGL task)
 */
#include "esp_log.h"
#include "waveshare_rgb_lcd_port.h"
#include "sd_card.h"
#include "i2c_io.h"
#include "hmi.h"
#include "can_if.h"
#include "canmap.h"
#include "datalog.h"
#include "settings.h"

static const char *TAG = "app_main";

void app_main(void)
{
    settings_init();            /* NVS; defaults on failure. Read by hmi_init and datalog */
    i2c_io_init();

    /* The map is read from the SD card and must be loaded before the UI
     * starts: the bridge resolves its bindings in hmi_init(). */
    if (waveshare_sd_card_init() != ESP_OK) {
        ESP_LOGW(TAG, "no SD card: built-in map; insert a card and use Settings > Retry SD to log");
    }
    ESP_ERROR_CHECK(canmap_load());

    lvgl_port_set_ui_hooks(hmi_init, hmi_tick);
    waveshare_esp32_s3_rgb_lcd_init();      /* starts the LVGL task -> hmi_init() */
    wavesahre_rgb_lcd_bl_on();
    ESP_LOGI(TAG, "display up");

    /* GPIO19/20 leave the USB port for the CAN transceiver from here on. */
    ESP_ERROR_CHECK(board_select_can(true));
    canmap_info_t mi;
    canmap_get_info(&mi);
    can_if_config_t can_cfg = CAN_IF_CONFIG_DASH_DEFAULT();
    if (mi.bitrate != 0) {
        can_cfg.bitrate = mi.bitrate;
    }
    ESP_ERROR_CHECK(can_if_init(&can_cfg));
    ESP_ERROR_CHECK(canmap_start());
    app_settings_t cfg;
    settings_get(&cfg);
    ESP_ERROR_CHECK(datalog_init(MOUNT_POINT, cfg.log_raw, cfg.log_params, waveshare_sd_card_remount));

    ESP_LOGI(TAG, "running");
}

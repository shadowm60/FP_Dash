#ifndef _NVS_HANDLER_H_
#define _NVS_HANDLER_H_

#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"

typedef enum {
    RUSEFI_CAN = 0,
    HALTECH_CAN = 1,
    /* TBD */
} kmatrix_id_enum;

typedef struct {
    uint8_t uds_source : 1;  // 1 -> UDS is used for input data, 0 -> kmatrix
    uint8_t kmatrix_id : 7;  // check kmatrix_id_enum;
} settings_src_t;

typedef struct {
    uint32_t rusefi_base_id; //broadcast base can
    settings_src_t data_source;
    
} settings_data_t;


typedef struct {
    uint8_t use_led : 1; // 1 for using the RGB led
    uint8_t use_auto_brightness : 1; // use the onboard brightness level
    uint8_t is_ts_calibrated : 1; // 1 -> TS is calibrated
    uint8_t use_sdlog : 1; // use sdcard for traffic logging
    uint8_t rerserved : 4;
} syst_bitfld_t;

typedef struct {
    syst_bitfld_t syst_bitfld;
    uint8_t bl_user_settings; //0..100%
    uint8_t active_screen; //store current screen to use it on next boot
} system_data_t;

typedef struct {

} nvs_data_t;

#endif
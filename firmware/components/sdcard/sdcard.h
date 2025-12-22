#ifndef _SDCARD_H_
#define _SDCARD_H_

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"


#define MOUNT_POINT "/sdcard"

#define CONFIG_SDCARD_PIN_CLK       18
#define CONFIG_SDCARD_PIN_MOSI      23
#define CONFIG_SDCARD_PIN_MISO      19
#define CONFIG_SDCARD_PIN_CS        5

#define MAX_LINE_CHAR_SIZE      255


void sdcard_init(void);
void sdcard_startTask(void);

#endif
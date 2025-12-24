idf_component_register(SRCS "lcd.c"
                    INCLUDE_DIRS "."
                    REQUIRES "hw_config" "driver" "esp_timer" "esp_lcd" "esp_lcd_touch_gt911" "lvgl" "gui"
                    )
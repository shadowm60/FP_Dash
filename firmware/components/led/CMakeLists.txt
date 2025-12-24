idf_component_register(SRCS "led.c" 
                    INCLUDE_DIRS "."
                    REQUIRES "hw_config" "driver" "esp_timer"
                    )
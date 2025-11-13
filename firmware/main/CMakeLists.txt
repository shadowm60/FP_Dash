idf_component_register(SRCS "can.c" "main.c" "lcd.c" "led.c"
                    INCLUDE_DIRS "."
                    REQUIRES "gui" "obd_handler" "driver" "gui_handlers")
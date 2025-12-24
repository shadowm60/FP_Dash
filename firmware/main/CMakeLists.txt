idf_component_register(SRCS "main.c" 
                    INCLUDE_DIRS "."
                    REQUIRES "gui" "lcd" "can" "led" "obd_handler" "driver" "gui_handlers")
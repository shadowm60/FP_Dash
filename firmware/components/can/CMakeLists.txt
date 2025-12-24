idf_component_register(SRCS "can.c"
                    INCLUDE_DIRS "."
                    REQUIRES "hw_config" "driver" "led" "isotp"
                    )
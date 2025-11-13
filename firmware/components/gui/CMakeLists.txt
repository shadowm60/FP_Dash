idf_component_register(SRCS "./fp_dash/src/ui/images.c" 
                            "./fp_dash/src/ui/screens.c" 
                            "./fp_dash/src/ui/styles.c" 
                            "./fp_dash/src/ui/ui.c"
                    INCLUDE_DIRS "./fp_dash/src/ui"
                    REQUIRES lvgl
                    )
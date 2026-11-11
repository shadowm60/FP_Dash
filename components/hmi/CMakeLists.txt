# UI: EEZ Studio generated code (src/ui, do not edit by hand, regenerate with
# tools/eez-build.sh) + bridge (hand written, the only code writing EEZ variables).
set(var_table "${CMAKE_CURRENT_BINARY_DIR}/eez_var_table.c")

idf_component_register(SRCS "./src/ui/images.c"
                            "./src/ui/screens.c"
                            "./src/ui/styles.c"
                            "./src/ui/ui.c"
                            "./src/ui/eez-flow.cpp"
                            "./bridge/eez_vars.cpp"
                            "./bridge/ui_bridge.c"
                            "./bridge/nav.c"
                            "./bridge/settings_view.c"
                            "./bridge/brightness.c"
                            "./bridge/actions.c"
                            "${var_table}"
                    INCLUDE_DIRS "./bridge"
                    PRIV_INCLUDE_DIRS "./src/ui"
                    REQUIRES lvgl
                    PRIV_REQUIRES canmap channels can_if datalog settings esp_app_format esp_driver_ledc)

# EEZ variable and screen names -> indices, from the generated vars.h / screens.h
idf_build_get_property(python PYTHON)
set(gen_script "${CMAKE_CURRENT_LIST_DIR}/../../tools/gen_eez_var_table.py")
set(vars_h "${CMAKE_CURRENT_LIST_DIR}/src/ui/vars.h")
set(screens_h "${CMAKE_CURRENT_LIST_DIR}/src/ui/screens.h")
add_custom_command(OUTPUT "${var_table}"
                   COMMAND ${python} "${gen_script}" "${vars_h}" "${screens_h}" "${var_table}"
                   DEPENDS "${vars_h}" "${screens_h}" "${gen_script}"
                   COMMENT "Generating EEZ variable / screen tables"
                   VERBATIM)

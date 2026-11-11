# SD card logging: raw CAN (candump format), decoded signals (MegaLogViewer CSV), events.
idf_component_register(SRCS "datalog.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES can_if canmap channels vfs)

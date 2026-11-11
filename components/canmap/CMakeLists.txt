# Signal map (XML from tools/fpdash_mapper) + CAN frame decoder.
# default_map.xml is the built-in fallback (rusEFI broadcast); regenerate it with
#   python3 tools/fpdash_mapper/fpdash_mapper.py --dbc tools/dbc/rusEFI_CAN_verbose.dbc \
#       --base components/canmap/default_map.xml \
#       --eez components/hmi/FP_Dash.eez-project --auto -o components/canmap/default_map.xml
idf_component_register(SRCS "canmap.c" "mini_xml.c"
                    INCLUDE_DIRS "."
                    REQUIRES channels
                    PRIV_REQUIRES can_if
                    EMBED_TXTFILES "default_map.xml")

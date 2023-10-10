idf_component_register(SRCS "./modules/aircomm.c"
                       INCLUDE_DIRS "./modules/inc"
                       REQUIRES esp_timer esp_lvgl_port driver)

idf_component_get_property(FREERTOS_ORIG_INCLUDE_PATH freertos ORIG_INCLUDE_PATH)
target_include_directories(${COMPONENT_TARGET} PUBLIC
    "${FREERTOS_ORIG_INCLUDE_PATH}")

target_compile_options(${COMPONENT_LIB} PRIVATE "-fno-strict-aliasing")

idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES
        hubo_bsp
        config_manager
        ina3221
        nvs_flash
        freertos
        esp_log
)

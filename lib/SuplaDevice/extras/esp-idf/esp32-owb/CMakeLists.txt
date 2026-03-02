idf_component_register(SRCS owb.c owb_gpio.c
                    INCLUDE_DIRS include
                    PRIV_REQUIRES esp_rom
                    REQUIRES esp_driver_gpio
                    )


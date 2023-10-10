/**
 * @file main.c
 * @brief Unit Testing of AIRCOMM Module.
 */

/*
 * Copyright (c) 2023 Murat Kös.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution.
 */

#include <stdbool.h>
#include <stdio.h>

#include "nvs_flash.h"

#include "aircomm.h"

void app_main( ) {
    /* Initialize nvs flash prepare for Wi-Fi. */
    esp_err_t retCode = nvs_flash_init( );
    if ( ( retCode == ESP_ERR_NVS_NO_FREE_PAGES ) || ( retCode == ESP_ERR_NVS_NEW_VERSION_FOUND ) ) {
        ESP_ERROR_CHECK( nvs_flash_erase( ) );
        retCode = nvs_flash_init( );
    }
    ESP_ERROR_CHECK( retCode );

    /* Launch the system task. */
    aircomm_launch( );

    __unreachable( );
}

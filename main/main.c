/**
 * @file main.c
 * @brief ...
 */

/*
 * Copyright (c) 2023 Murat Kös.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

static const char *TAG = "unit_test";

void app_main(void)
{
    while (1) {
        ESP_LOGI(TAG, "unit testing...");

        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}

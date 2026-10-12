/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * ENV Sea-Level Pressure Calibration by Known Pressure
 *
 * This example directly sets the sea-level pressure used by the SPA06
 * altitude calculation. Use it when the current local sea-level pressure is
 * already known from a reliable weather station, official weather service, or
 * another trusted reference.
 *
 * Steps:
 * 1. Obtain the current local sea-level pressure for the ENV module location.
 * 2. Convert the pressure to hPa multiplied by 10 and set
 *    sea_level_pressure_hpa_x10 below. For example, 10144 means 1014.4 hPa.
 * 3. Upload and run this example once.
 * 4. This example uses CHAIN_SAVE_FLASH_ENABLE, so the setting is saved to
 *    Flash and restored automatically after the next power-up.
 * 5. Check the serial output and the readback value to confirm the setting.
 *
 * The supported sea-level pressure range is 300.0 to 1100.0 hPa. This feature
 * changes the altitude calculation reference and does not calibrate the
 * internal pressure-sensor coefficients. Avoid repeated Flash writes.
 */

#include "M5Chain.h"

#define TXD_PIN GPIO_NUM_21
#define RXD_PIN GPIO_NUM_22

Chain M5Chain;
device_list_t *devices_list = nullptr;

static bool getEnvDevices(void)
{
    uint16_t device_nums = 0;

    if (!M5Chain.isDeviceConnected() || M5Chain.getDeviceNum(&device_nums) != CHAIN_OK) {
        return false;
    }

    devices_list = (device_list_t *)malloc(sizeof(device_list_t));
    if (devices_list == nullptr) {
        return false;
    }

    devices_list->count   = device_nums;
    devices_list->devices = (device_info_t *)malloc(sizeof(device_info_t) * device_nums);
    if (devices_list->devices == nullptr || !M5Chain.getDeviceList(devices_list)) {
        return false;
    }

    return true;
}

void setup()
{
    const uint16_t sea_level_pressure_hpa_x10 = 10144;  // 1014.4 hPa
    uint8_t operation_status                  = 0;

    Serial.begin(115200);
    M5Chain.begin(&Serial2, 115200, RXD_PIN, TXD_PIN);

    if (!getEnvDevices()) {
        Serial.println("ENV device enumeration failed");
        return;
    }

    for (uint8_t i = 0; i < devices_list->count; i++) {
        if (devices_list->devices[i].device_type != CHAIN_ENV_TYPE_CODE) {
            continue;
        }

        uint8_t id            = devices_list->devices[i].id;
        chain_status_t status = M5Chain.setSPA06SeaLevelPressure(id, sea_level_pressure_hpa_x10, &operation_status,
                                                                 CHAIN_SAVE_FLASH_ENABLE);

        if (status == CHAIN_OK && operation_status == 1) {
            Serial.printf("ENV ID[%d] sea-level pressure set to %.1f hPa\r\n", id, sea_level_pressure_hpa_x10 / 10.0f);
        } else {
            Serial.printf("ENV ID[%d] setting failed, chain_status: %d, operation_status: %d\r\n", id, status,
                          operation_status);
        }

        uint16_t actual_pressure_hpa_x10 = 0;
        status                           = M5Chain.getSPA06SeaLevelPressure(id, &actual_pressure_hpa_x10);
        if (status == CHAIN_OK) {
            Serial.printf("ENV ID[%d] current sea-level pressure: %.1f hPa\r\n", id, actual_pressure_hpa_x10 / 10.0f);
        } else {
            Serial.printf("ENV ID[%d] readback failed, chain_status: %d\r\n", id, status);
        }
    }
}

void loop()
{
    delay(1000);
}

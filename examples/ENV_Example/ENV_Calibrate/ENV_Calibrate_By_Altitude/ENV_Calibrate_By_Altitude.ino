/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * ENV Sea-Level Pressure Calibration by Reference Altitude
 *
 * This example calibrates the sea-level pressure used by the SPA06 altitude
 * calculation. It is suitable when the ENV module is placed at a location
 * with a known altitude, but the local sea-level pressure is not available.
 *
 * Steps:
 * 1. Confirm the actual altitude of the ENV module installation location.
 * 2. Set reference_altitude_cm below to that altitude in centimeters.
 *    For example, 5000 means 50.00 meters.
 * 3. Upload and run this example once. The firmware samples SPA06 pressure
 *    16 times, averages the readings, and calculates the sea-level pressure.
 * 4. This example uses CHAIN_SAVE_FLASH_ENABLE, so the calculated value is
 *    saved to Flash and restored automatically after the next power-up.
 * 5. Check the serial output for the calibration result and the readback
 *    sea-level pressure. Keep the module still during the approximately
 *    4-second sampling period.
 *
 * The reference altitude must be the altitude of the sensor location. This
 * feature calibrates the altitude calculation reference and does not calibrate
 * the internal pressure-sensor coefficients. Avoid repeated Flash writes.
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
    const int32_t reference_altitude_cm = 5000;
    uint8_t operation_status            = 0;

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
        chain_status_t status = M5Chain.calibrateSPA06SeaLevelPressure(id, reference_altitude_cm, &operation_status,
                                                                       CHAIN_SAVE_FLASH_ENABLE, 10000);

        if (status == CHAIN_OK && operation_status == 1) {
            Serial.printf("ENV ID[%d] calibration succeeded, reference altitude: %ld cm\r\n", id,
                          reference_altitude_cm);
        } else {
            Serial.printf("ENV ID[%d] calibration failed, chain_status: %d, operation_status: %d\r\n", id, status,
                          operation_status);
        }

        uint16_t pressure_hpa_x10 = 0;
        status                    = M5Chain.getSPA06SeaLevelPressure(id, &pressure_hpa_x10);
        if (status == CHAIN_OK) {
            Serial.printf("ENV ID[%d] sea-level pressure: %.1f hPa\r\n", id, pressure_hpa_x10 / 10.0f);
        }
    }
}

void loop()
{
    delay(1000);
}

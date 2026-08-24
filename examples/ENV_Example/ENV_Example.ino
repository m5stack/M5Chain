/*
 *SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#include "M5Chain.h"

#define TXD_PIN GPIO_NUM_21  // Tx
#define RXD_PIN GPIO_NUM_22  // Rx

Chain M5Chain;

device_list_t *devices_list = NULL;
uint16_t device_nums        = 0;
uint8_t operation_status    = 0;
chain_status_t chain_status = CHAIN_OK;

uint8_t rgb_test[5][3] = {
    {0xFF, 0x00, 0x00}, {0x00, 0xFF, 0x00}, {0x00, 0x00, 0xFF}, {0xFF, 0xFF, 0xFF}, {0x00, 0x00, 0x00},
};

uint8_t uid32[4]  = {0};
uint8_t uid96[12] = {0};

int16_t temperature_x100 = 0;
uint8_t humidity         = 0;
uint32_t pressure_x10    = 0;
int32_t altitude_cm      = 0;

uint8_t env_trigger_mode                   = 0x07;  // bit 0: temperature, bit 1: humidity, bit 2: pressure, bit 3:
uint8_t env_temperature_low_threshold      = 0;
uint8_t env_temperature_high_threshold     = 35;
uint8_t env_humidity_low_threshold         = 20;
uint8_t env_humidity_high_threshold        = 70;
uint16_t env_pressure_low_threshold_x10    = 10000;
uint16_t env_pressure_high_threshold_x10   = 10500;
uint16_t env_sea_level_pressure_x10        = 10120;
int32_t env_altitude_low_threshold_cm      = 2000;
int32_t env_altitude_high_threshold_cm     = 110000;
uint8_t env_aht20_i2c_addr                 = 0x38;
uint8_t env_aht20_status_reg               = 0x71;
uint8_t env_aht20_measure_reg              = 0xAC;
bool env_event_trigger_last_valid[256]     = {false};
uint8_t env_event_trigger_last_status[256] = {0};

void printDeviceList(device_list_t *devices)
{
    if (devices == NULL) {
        Serial.println("devices is NULL");
        return;
    }

    Serial.print("devices count: ");
    Serial.println(devices->count);

    for (uint8_t i = 0; i < devices->count; i++) {
        Serial.print("devices ID: ");
        Serial.println(devices->devices[i].id);
        Serial.print("devices type: ");
        Serial.println(devices->devices[i].device_type);
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("M5Chain Dlight Test");
    M5Chain.begin(&Serial2, 115200, RXD_PIN, TXD_PIN);

    if (M5Chain.isDeviceConnected()) {
        Serial.println("devices is connected");
        chain_status = M5Chain.getDeviceNum(&device_nums);
        if (chain_status == CHAIN_OK) {
            devices_list          = (device_list_t *)malloc(sizeof(device_list_t));
            devices_list->count   = device_nums;
            devices_list->devices = (device_info_t *)malloc(sizeof(device_info_t) * device_nums);
            if (M5Chain.getDeviceList(devices_list)) {
                Serial.println("get devices list success");
                printDeviceList(devices_list);
            } else {
                Serial.println("get devices list failed");
            }
        } else {
            Serial.printf("error status:%d \r\n", chain_status);
            Serial.printf("devices num get failed.\r\n");
        }
    } else {
        Serial.println("devices is not connected.");
    }

    if (devices_list) {
        for (uint8_t i = 0; i < devices_list->count; i++) {
            if (devices_list->devices[i].device_type == CHAIN_ENV_TYPE_CODE) {
                chain_status = M5Chain.setRGBLight(devices_list->devices[i].id, 100, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] set rgb light success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("ID[%d] set rgb light failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }
                for (uint8_t j = 0; j < 5; j++) {
                    uint8_t rgb[3] = {0};
                    chain_status =
                        M5Chain.setRGBValue(devices_list->devices[i].id, 0, 1, rgb_test[j], 3, &operation_status);
                    if (chain_status == CHAIN_OK && operation_status) {
                        Serial.printf("ID[%d] set rgb %d %d %d success\r\n", devices_list->devices[i].id,
                                      rgb_test[j][0], rgb_test[j][1], rgb_test[j][2]);
                    } else {
                        Serial.printf("ID[%d] set rgb %d %d %d failed, chain_status:%d  operation_status:%d \r\n",
                                      devices_list->devices[i].id, rgb_test[j][0], rgb_test[j][1], rgb_test[j][2],
                                      chain_status, operation_status);
                    }
                    chain_status = M5Chain.getRGBValue(devices_list->devices[i].id, 0, 1, rgb, 3, &operation_status);
                    if (chain_status == CHAIN_OK && operation_status) {
                        Serial.printf("ID[%d] get rgb %d %d %d success \r\n", devices_list->devices[i].id, rgb[0],
                                      rgb[1], rgb[2]);
                    } else {
                        Serial.printf("ID[%d] get rgb %d %d %d failed, chain_status:%d  operation_status:%d \r\n",
                                      devices_list->devices[i].id, rgb[0], rgb[1], rgb[2], chain_status,
                                      operation_status);
                    }
                    delay(500);
                }

                Serial.printf("ID[%d] Chain ENV type code: 0x%04x\r\n", devices_list->devices[i].id,
                              M5Chain.getENVTypeCode());

                uint8_t aht20_status = 0;
                operation_status     = 0;
                chain_status =
                    M5Chain.envI2cMemRead(devices_list->devices[i].id, env_aht20_i2c_addr, env_aht20_status_reg,
                                          CHAIN_ENV_I2C_MEMADD_SIZE_8BIT, 1, &aht20_status, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] env i2c mem read success, aht20_status:0x%02x\r\n",
                                  devices_list->devices[i].id, aht20_status);
                } else {
                    Serial.printf("ID[%d] env i2c mem read failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                uint8_t aht20_measure_cmd[2] = {0x33, 0x00};
                operation_status             = 0;
                chain_status =
                    M5Chain.envI2cMemWrite(devices_list->devices[i].id, env_aht20_i2c_addr, env_aht20_measure_reg,
                                           CHAIN_ENV_I2C_MEMADD_SIZE_8BIT, 2, aht20_measure_cmd, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] env i2c mem write success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("ID[%d] env i2c mem write failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }
                delay(80);

                int16_t aht20_temperature_x100 = 0;
                uint8_t aht20_humidity         = 0;
                chain_status =
                    M5Chain.getAHT20Data(devices_list->devices[i].id, &aht20_temperature_x100, &aht20_humidity);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] get aht20 data success, temperature:%.2f C humidity:%u %%\r\n",
                                  devices_list->devices[i].id, aht20_temperature_x100 / 100.0f, aht20_humidity);
                } else {
                    Serial.printf("ID[%d] get aht20 data failed, chain_status:%d\r\n", devices_list->devices[i].id,
                                  chain_status);
                }

                int16_t spa06_temperature_x100 = 0;
                uint16_t spa06_pressure_x10    = 0;
                int32_t spa06_altitude_cm      = 0;
                chain_status = M5Chain.getSPA06Data(devices_list->devices[i].id, &spa06_temperature_x100,
                                                    &spa06_pressure_x10, &spa06_altitude_cm);
                if (chain_status == CHAIN_OK) {
                    Serial.printf(
                        "ID[%d] get spa06 data success, temperature:%.2f C pressure:%.1f hPa altitude:%ld cm\r\n",
                        devices_list->devices[i].id, spa06_temperature_x100 / 100.0f, spa06_pressure_x10 / 10.0f,
                        spa06_altitude_cm);
                } else {
                    Serial.printf("ID[%d] get spa06 data failed, chain_status:%d\r\n", devices_list->devices[i].id,
                                  chain_status);
                }

                operation_status = 0;
                chain_status = M5Chain.setSPA06TemperatureRate(devices_list->devices[i].id, CHAIN_ENV_SPA06_RATE_1HZ,
                                                               &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] set spa06 temperature rate success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf(
                        "ID[%d] set spa06 temperature rate failed, chain_status:%d operation_status : %d\r\n ",
                        devices_list->devices[i].id, chain_status, operation_status);
                }

                chain_env_spa06_rate_t spa06_temperature_rate = CHAIN_ENV_SPA06_RATE_2HZ;
                chain_status = M5Chain.getSPA06TemperatureRate(devices_list->devices[i].id, &spa06_temperature_rate);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] get spa06 temperature rate success, rate:%d\r\n", devices_list->devices[i].id,
                                  spa06_temperature_rate);
                } else {
                    Serial.printf("ID[%d] get spa06 temperature rate failed, chain_status:%d\r\n",
                                  devices_list->devices[i].id, chain_status);
                }

                operation_status = 0;
                chain_status     = M5Chain.setSPA06PressureRate(devices_list->devices[i].id, CHAIN_ENV_SPA06_RATE_1HZ,
                                                                &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] set spa06 pressure rate success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("ID[%d] set spa06 pressure rate failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                chain_env_spa06_rate_t spa06_pressure_rate = CHAIN_ENV_SPA06_RATE_2HZ;
                chain_status = M5Chain.getSPA06PressureRate(devices_list->devices[i].id, &spa06_pressure_rate);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] get spa06 pressure rate success, rate:%d\r\n", devices_list->devices[i].id,
                                  spa06_pressure_rate);
                } else {
                    Serial.printf("ID[%d] get spa06 pressure rate failed, chain_status:%d\r\n",
                                  devices_list->devices[i].id, chain_status);
                }

                operation_status = 0;
                chain_status = M5Chain.setSPA06SeaLevelPressure(devices_list->devices[i].id, env_sea_level_pressure_x10,
                                                                &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] set spa06 sea level pressure success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf(
                        "ID[%d] set spa06 sea level pressure failed, chain_status:%d  operation_status:%d \r\n",
                        devices_list->devices[i].id, chain_status, operation_status);
                }

                uint16_t spa06_sea_level_pressure_x10 = 0;
                chain_status =
                    M5Chain.getSPA06SeaLevelPressure(devices_list->devices[i].id, &spa06_sea_level_pressure_x10);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] get spa06 sea level pressure success, pressure:%.1f hPa\r\n",
                                  devices_list->devices[i].id, spa06_sea_level_pressure_x10 / 10.0f);
                } else {
                    Serial.printf("ID[%d] get spa06 sea level pressure failed, chain_status:%d\r\n",
                                  devices_list->devices[i].id, chain_status);
                }

                operation_status = 0;
                chain_status     = M5Chain.setENVChangeEventTriggerMode(devices_list->devices[i].id, env_trigger_mode,
                                                                        &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] set env change event trigger mode success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf(
                        "ID[%d] set env change event trigger mode failed, chain_status:%d  operation_status:%d \r\n",
                        devices_list->devices[i].id, chain_status, operation_status);
                }

                uint8_t trigger_mode = 0;
                chain_status         = M5Chain.getENVChangeEventTriggerMode(devices_list->devices[i].id, &trigger_mode);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] get env change event trigger mode success, mode:0x%02x\r\n",
                                  devices_list->devices[i].id, trigger_mode);
                } else {
                    Serial.printf("ID[%d] get env change event trigger mode failed, chain_status:%d\r\n",
                                  devices_list->devices[i].id, chain_status);
                }

                operation_status = 0;
                chain_status     = M5Chain.setENVTemperatureWarningThreshold(
                        devices_list->devices[i].id, CHAIN_ENV_TEMPERATURE_SOURCE_AHT20, env_temperature_low_threshold,
                        env_temperature_high_threshold, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] set env temperature warning threshold success\r\n",
                                  devices_list->devices[i].id);
                } else {
                    Serial.printf(
                        "ID[%d] set env temperature warning threshold failed, chain_status:%d  operation_status:%d "
                        "\r\n",
                        devices_list->devices[i].id, chain_status, operation_status);
                }

                chain_env_temperature_source_t temperature_source = CHAIN_ENV_TEMPERATURE_SOURCE_AHT20;
                uint8_t temperature_low_threshold                 = 0;
                uint8_t temperature_high_threshold                = 0;
                chain_status =
                    M5Chain.getENVTemperatureWarningThreshold(devices_list->devices[i].id, &temperature_source,
                                                              &temperature_low_threshold, &temperature_high_threshold);
                if (chain_status == CHAIN_OK) {
                    Serial.printf(
                        "ID[%d] get env temperature warning threshold success, source:%d low:%u C high:%u C\r\n",
                        devices_list->devices[i].id, temperature_source, temperature_low_threshold,
                        temperature_high_threshold);
                } else {
                    Serial.printf("ID[%d] get env temperature warning threshold failed, chain_status:%d\r\n",
                                  devices_list->devices[i].id, chain_status);
                }

                operation_status = 0;
                chain_status =
                    M5Chain.setENVHumidityWarningThreshold(devices_list->devices[i].id, env_humidity_low_threshold,
                                                           env_humidity_high_threshold, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] set env humidity warning threshold success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf(
                        "ID[%d] set env humidity warning threshold failed, chain_status:%d  operation_status:%d\r\n",
                        devices_list->devices[i].id, chain_status, operation_status);
                }

                uint8_t humidity_low_threshold  = 0;
                uint8_t humidity_high_threshold = 0;
                chain_status                    = M5Chain.getENVHumidityWarningThreshold(
                                       devices_list->devices[i].id, &humidity_low_threshold, &humidity_high_threshold);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] get env humidity warning threshold success, low:%u %% high:%u %%\r\n",
                                  devices_list->devices[i].id, humidity_low_threshold, humidity_high_threshold);
                } else {
                    Serial.printf("ID[%d] get env humidity warning threshold failed, chain_status:%d\r\n",
                                  devices_list->devices[i].id, chain_status);
                }

                operation_status = 0;
                chain_status =
                    M5Chain.setENVPressureWarningThreshold(devices_list->devices[i].id, env_pressure_low_threshold_x10,
                                                           env_pressure_high_threshold_x10, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] set env pressure warning threshold success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf(
                        "ID[%d] set env pressure warning threshold failed, chain_status:%d  operation_status:%d\r\n",
                        devices_list->devices[i].id, chain_status, operation_status);
                }

                uint16_t pressure_low_threshold_x10  = 0;
                uint16_t pressure_high_threshold_x10 = 0;
                chain_status                         = M5Chain.getENVPressureWarningThreshold(
                                            devices_list->devices[i].id, &pressure_low_threshold_x10, &pressure_high_threshold_x10);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] get env pressure warning threshold success, low:%.1f hPa high:%.1fhPa\r\n",
                                  devices_list->devices[i].id, pressure_low_threshold_x10 / 10.0f,
                                  pressure_high_threshold_x10 / 10.0f);
                } else {
                    Serial.printf("ID[%d] get env pressure warning threshold failed, chain_status:%d\r\n",
                                  devices_list->devices[i].id, chain_status);
                }

                operation_status = 0;
                chain_status =
                    M5Chain.setENVAltitudeWarningThreshold(devices_list->devices[i].id, env_altitude_low_threshold_cm,
                                                           env_altitude_high_threshold_cm, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] set env altitude warning threshold success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf(
                        "ID[%d] set env altitude warning threshold failed, chain_status:%d  operation_status:%d\r\n",
                        devices_list->devices[i].id, chain_status, operation_status);
                }

                int32_t altitude_low_threshold_cm  = 0;
                int32_t altitude_high_threshold_cm = 0;
                chain_status                       = M5Chain.getENVAltitudeWarningThreshold(
                                          devices_list->devices[i].id, &altitude_low_threshold_cm, &altitude_high_threshold_cm);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] get env altitude warning threshold success, low:%ld cm high:%ld cm\r\n",
                                  devices_list->devices[i].id, altitude_low_threshold_cm, altitude_high_threshold_cm);
                } else {
                    Serial.printf("ID[%d] get env altitude warning threshold failed, chain_status:%d\r\n",
                                  devices_list->devices[i].id, chain_status);
                }

                uint8_t env_trigger_status = 0;
                if (M5Chain.getENVChangeTrigger(devices_list->devices[i].id, &env_trigger_status)) {
                    Serial.printf("ID[%d] get env change trigger success, trigger_status:0x%02x\r\n",
                                  devices_list->devices[i].id, env_trigger_status);
                } else {
                    Serial.printf("ID[%d] get env change trigger no record\r\n", devices_list->devices[i].id);
                }
            }
        }
    } else {
        Serial.println("devices list is NULL");
    }
}

void loop()
{
    if (devices_list == NULL) {
        delay(500);
        return;
    }

    for (uint8_t i = 0; i < devices_list->count; i++) {
        if (devices_list->devices[i].device_type != CHAIN_ENV_TYPE_CODE) {
            continue;
        }

        uint8_t id = devices_list->devices[i].id;

        chain_status = M5Chain.getTemperatureData(id, &temperature_x100);
        if (chain_status == CHAIN_OK) {
            Serial.printf("ID[%d] temperature: %.2f C\r\n", id, temperature_x100 / 100.0f);
        } else {
            Serial.printf("ID[%d] get temperature failed, chain_status:%d\r\n", id, chain_status);
        }

        chain_status = M5Chain.getHumidityData(id, &humidity);
        if (chain_status == CHAIN_OK) {
            Serial.printf("ID[%d] humidity: %u %%\r\n", id, humidity);
        } else {
            Serial.printf("ID[%d] get humidity failed, chain_status:%d\r\n", id, chain_status);
        }

        chain_status = M5Chain.getPressureData(id, &pressure_x10);
        if (chain_status == CHAIN_OK) {
            Serial.printf("ID[%d] pressure: %.2f hPa\r\n", id, pressure_x10 / 10.0f);
        } else {
            Serial.printf("ID[%d] get pressure failed, chain_status:%d\r\n", id, chain_status);
        }

        chain_status = M5Chain.getAltitudeData(id, &altitude_cm);
        if (chain_status == CHAIN_OK) {
            Serial.printf("ID[%d] altitude: %ld cm\r\n", id, altitude_cm);
        } else {
            Serial.printf("ID[%d] get altitude failed, chain_status:%d\r\n", id, chain_status);
        }

        uint8_t env_trigger_status = 0;
        while (M5Chain.getENVChangeTrigger(id, &env_trigger_status, 100)) {
            uint16_t event_trigger_res = CHAIN_ENV_EVENT_TRIGGER_REPORT_TYPE | env_trigger_status;

            if ((event_trigger_res & CHAIN_ENV_EVENT_TRIGGER_REPORT_MASK) != CHAIN_ENV_EVENT_TRIGGER_REPORT_TYPE) {
                continue;
            }

            uint8_t changed_status = env_trigger_status ^ env_event_trigger_last_status[id];
            if (!env_event_trigger_last_valid[id]) {
                changed_status = env_trigger_status;
            }

            env_event_trigger_last_valid[id]  = true;
            env_event_trigger_last_status[id] = env_trigger_status;

            if (changed_status == 0) {
                continue;
            }

            if (changed_status & CHAIN_ENV_EVENT_STATUS_TEMPERATURE_MASK) {
                switch (event_trigger_res & CHAIN_ENV_EVENT_TRIGGER_TEMPERATURE_MASK) {
                    case CHAIN_ENV_EVENT_TRIGGER_TEMPERATURE_HIGH_RELEASE:
                        Serial.printf("ENV ID[%d] temperature trigger result is: high release\r\n", id);
                        break;
                    case CHAIN_ENV_EVENT_TRIGGER_TEMPERATURE_HIGH_ACTIVE:
                        Serial.printf("ENV ID[%d] temperature trigger result is: high active\r\n", id);
                        break;
                    case CHAIN_ENV_EVENT_TRIGGER_TEMPERATURE_LOW_ACTIVE:
                        Serial.printf("ENV ID[%d] temperature trigger result is: low active\r\n", id);
                        break;
                    case CHAIN_ENV_EVENT_TRIGGER_TEMPERATURE_LOW_RELEASE:
                        Serial.printf("ENV ID[%d] temperature trigger result is: low release\r\n", id);
                        break;
                    default:
                        Serial.printf("ENV ID[%d] temperature trigger result is: unknown, status:0x%02x\r\n", id,
                                      env_trigger_status);
                        break;
                }
            }

            if (changed_status & CHAIN_ENV_EVENT_STATUS_HUMIDITY_MASK) {
                switch (event_trigger_res & CHAIN_ENV_EVENT_TRIGGER_HUMIDITY_MASK) {
                    case CHAIN_ENV_EVENT_TRIGGER_HUMIDITY_HIGH_RELEASE:
                        Serial.printf("ENV ID[%d] humidity trigger result is: high release\r\n", id);
                        break;
                    case CHAIN_ENV_EVENT_TRIGGER_HUMIDITY_HIGH_ACTIVE:
                        Serial.printf("ENV ID[%d] humidity trigger result is: high active\r\n", id);
                        break;
                    case CHAIN_ENV_EVENT_TRIGGER_HUMIDITY_LOW_ACTIVE:
                        Serial.printf("ENV ID[%d] humidity trigger result is: low active\r\n", id);
                        break;
                    case CHAIN_ENV_EVENT_TRIGGER_HUMIDITY_LOW_RELEASE:
                        Serial.printf("ENV ID[%d] humidity trigger result is: low release\r\n", id);
                        break;
                    default:
                        Serial.printf("ENV ID[%d] humidity trigger result is: unknown, status:0x%02x\r\n", id,
                                      env_trigger_status);
                        break;
                }
            }

            if (changed_status & CHAIN_ENV_EVENT_STATUS_PRESSURE_MASK) {
                switch (event_trigger_res & CHAIN_ENV_EVENT_TRIGGER_PRESSURE_MASK) {
                    case CHAIN_ENV_EVENT_TRIGGER_PRESSURE_HIGH_RELEASE:
                        Serial.printf("ENV ID[%d] pressure trigger result is: high release\r\n", id);
                        break;
                    case CHAIN_ENV_EVENT_TRIGGER_PRESSURE_HIGH_ACTIVE:
                        Serial.printf("ENV ID[%d] pressure trigger result is: high active\r\n", id);
                        break;
                    case CHAIN_ENV_EVENT_TRIGGER_PRESSURE_LOW_ACTIVE:
                        Serial.printf("ENV ID[%d] pressure trigger result is: low active\r\n", id);
                        break;
                    case CHAIN_ENV_EVENT_TRIGGER_PRESSURE_LOW_RELEASE:
                        Serial.printf("ENV ID[%d] pressure trigger result is: low release\r\n", id);
                        break;
                    default:
                        Serial.printf("ENV ID[%d] pressure trigger result is: unknown, status:0x%02x\r\n", id,
                                      env_trigger_status);
                        break;
                }
            }

            if (changed_status & CHAIN_ENV_EVENT_STATUS_ALTITUDE_MASK) {
                switch (event_trigger_res & CHAIN_ENV_EVENT_TRIGGER_ALTITUDE_MASK) {
                    case CHAIN_ENV_EVENT_TRIGGER_ALTITUDE_HIGH_RELEASE:
                        Serial.printf("ENV ID[%d] altitude trigger result is: high release\r\n", id);
                        break;
                    case CHAIN_ENV_EVENT_TRIGGER_ALTITUDE_HIGH_ACTIVE:
                        Serial.printf("ENV ID[%d] altitude trigger result is: high active\r\n", id);
                        break;
                    case CHAIN_ENV_EVENT_TRIGGER_ALTITUDE_LOW_ACTIVE:
                        Serial.printf("ENV ID[%d] altitude trigger result is: low active\r\n", id);
                        break;
                    case CHAIN_ENV_EVENT_TRIGGER_ALTITUDE_LOW_RELEASE:
                        Serial.printf("ENV ID[%d] altitude trigger result is: low release\r\n", id);
                        break;
                    default:
                        Serial.printf("ENV ID[%d] altitude trigger result is: unknown, status:0x%02x\r\n", id,
                                      env_trigger_status);
                        break;
                }
            }
        }
    }

    delay(1000);
}

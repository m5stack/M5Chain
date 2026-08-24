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

int16_t accel_x = 0, accel_y = 0, accel_z = 0;
int16_t gyro_x = 0, gyro_y = 0, gyro_z = 0;
int16_t accel_raw_x = 0, accel_raw_y = 0, accel_raw_z = 0;
int16_t gyro_raw_x = 0, gyro_raw_y = 0, gyro_raw_z = 0;
uint16_t imu_temperature_x100 = 0;

uint8_t imu_sensor_i2c_addr = 0x6B;
uint8_t imu_who_am_i_reg    = 0x0F;
uint8_t imu_ctrl3c_reg      = 0x12;
uint8_t imu_event_trigger_mode =
    CHAIN_IMU_EVENT_MODE_ACCEL | CHAIN_IMU_EVENT_MODE_GYRO | CHAIN_IMU_EVENT_MODE_TEMPERATURE;
uint8_t imu_accel_warning_threshold             = 40;
uint8_t imu_gyro_warning_threshold              = 40;
uint8_t imu_temperature_low_threshold           = 0;
uint8_t imu_temperature_high_threshold          = 30;
bool imu_event_trigger_last_valid[256]          = {false};
uint8_t imu_event_trigger_last_status[256]      = {0};
uint8_t imu_event_trigger_last_report_code[256] = {0};

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
    Serial.println("M5Chain IMU Test");
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
            if (devices_list->devices[i].device_type == CHAIN_IMU_TYPE_CODE) {
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

                Serial.printf("ID[%d] Chain IMU type code: 0x%04x\r\n", devices_list->devices[i].id,
                              M5Chain.getIMUTypeCode());

                uint8_t who_am_i = 0;
                operation_status = 0;
                chain_status = M5Chain.imuI2cMemRead(devices_list->devices[i].id, imu_sensor_i2c_addr, imu_who_am_i_reg,
                                                     CHAIN_IMU_I2C_MEMADD_SIZE_8BIT, 1, &who_am_i, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] imu i2c mem read success, who_am_i:0x%02x\r\n", devices_list->devices[i].id,
                                  who_am_i);
                } else {
                    Serial.printf("ID[%d] imu i2c mem read failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                uint8_t ctrl3c_data = 0;
                operation_status    = 0;
                chain_status =
                    M5Chain.imuI2cMemRead(devices_list->devices[i].id, imu_sensor_i2c_addr, imu_ctrl3c_reg,
                                          CHAIN_IMU_I2C_MEMADD_SIZE_8BIT, 1, &ctrl3c_data, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    operation_status = 0;
                    chain_status =
                        M5Chain.imuI2cMemWrite(devices_list->devices[i].id, imu_sensor_i2c_addr, imu_ctrl3c_reg,
                                               CHAIN_IMU_I2C_MEMADD_SIZE_8BIT, 1, &ctrl3c_data, &operation_status);
                    if (chain_status == CHAIN_OK && operation_status) {
                        Serial.printf("ID[%d] imu i2c mem write success\r\n", devices_list->devices[i].id);
                    } else {
                        Serial.printf("ID[%d] imu i2c mem write failed, chain_status:%d  operation_status:%d \r\n",
                                      devices_list->devices[i].id, chain_status, operation_status);
                    }
                } else {
                    Serial.printf("ID[%d] imu i2c mem write skipped, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                chain_status = M5Chain.getIMUAccelData(devices_list->devices[i].id, &accel_x, &accel_y, &accel_z);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] get imu accel data success, x:%d mg y:%d mg z:%d mg\r\n",
                                  devices_list->devices[i].id, accel_x, accel_y, accel_z);
                } else {
                    Serial.printf("ID[%d] get imu accel data failed, chain_status:%d\r\n", devices_list->devices[i].id,
                                  chain_status);
                }

                chain_status = M5Chain.getIMUGyroData(devices_list->devices[i].id, &gyro_x, &gyro_y, &gyro_z);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] get imu gyro data success, x:%d dps y:%d dps z:%d dps\r\n",
                                  devices_list->devices[i].id, gyro_x, gyro_y, gyro_z);
                } else {
                    Serial.printf("ID[%d] get imu gyro data failed, chain_status:%d\r\n", devices_list->devices[i].id,
                                  chain_status);
                }

                chain_status =
                    M5Chain.getIMUAccelRawData(devices_list->devices[i].id, &accel_raw_x, &accel_raw_y, &accel_raw_z);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] get imu accel raw data success, x:%d y:%d z:%d\r\n",
                                  devices_list->devices[i].id, accel_raw_x, accel_raw_y, accel_raw_z);
                } else {
                    Serial.printf("ID[%d] get imu accel raw data failed, chain_status:%d\r\n",
                                  devices_list->devices[i].id, chain_status);
                }

                chain_status =
                    M5Chain.getIMUGyroRawData(devices_list->devices[i].id, &gyro_raw_x, &gyro_raw_y, &gyro_raw_z);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] get imu gyro raw data success, x:%d y:%d z:%d\r\n",
                                  devices_list->devices[i].id, gyro_raw_x, gyro_raw_y, gyro_raw_z);
                } else {
                    Serial.printf("ID[%d] get imu gyro raw data failed, chain_status:%d\r\n",
                                  devices_list->devices[i].id, chain_status);
                }

                chain_status = M5Chain.getIMUTemperature(devices_list->devices[i].id, &imu_temperature_x100);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] get imu temperature success, temperature:%.2f C\r\n",
                                  devices_list->devices[i].id, imu_temperature_x100 / 100.0f);
                } else {
                    Serial.printf("ID[%d] get imu temperature failed, chain_status:%d\r\n", devices_list->devices[i].id,
                                  chain_status);
                }

                operation_status = 0;
                chain_status =
                    M5Chain.setIMUAccelRange(devices_list->devices[i].id, CHAIN_IMU_ACCEL_RANGE_4G, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] set imu accel range success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("ID[%d] set imu accel range failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                chain_imu_accel_range_t accel_range = CHAIN_IMU_ACCEL_RANGE_2G;
                chain_status = M5Chain.getIMUAccelRange(devices_list->devices[i].id, &accel_range);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] get imu accel range success, range:%d\r\n", devices_list->devices[i].id,
                                  accel_range);
                } else {
                    Serial.printf("ID[%d] get imu accel range failed, chain_status:%d\r\n", devices_list->devices[i].id,
                                  chain_status);
                }

                operation_status = 0;
                chain_status     = M5Chain.setIMUGyroRange(devices_list->devices[i].id, CHAIN_IMU_GYRO_RANGE_500DPS,
                                                           &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] set imu gyro range success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("ID[%d] set imu gyro range failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                chain_imu_gyro_range_t gyro_range = CHAIN_IMU_GYRO_RANGE_250DPS;
                chain_status                      = M5Chain.getIMUGyroRange(devices_list->devices[i].id, &gyro_range);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] get imu gyro range success, range:%d\r\n", devices_list->devices[i].id,
                                  gyro_range);
                } else {
                    Serial.printf("ID[%d] get imu gyro range failed, chain_status:%d\r\n", devices_list->devices[i].id,
                                  chain_status);
                }

                operation_status = 0;
                chain_status =
                    M5Chain.setIMUAccelODR(devices_list->devices[i].id, CHAIN_IMU_ACCEL_ODR_104HZ, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] set imu accel odr success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("ID[%d] set imu accel odr failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                chain_imu_accel_odr_t accel_odr = CHAIN_IMU_ACCEL_ODR_OFF;
                chain_status                    = M5Chain.getIMUAccelODR(devices_list->devices[i].id, &accel_odr);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] get imu accel odr success, odr:%d\r\n", devices_list->devices[i].id,
                                  accel_odr);
                } else {
                    Serial.printf("ID[%d] get imu accel odr failed, chain_status:%d\r\n", devices_list->devices[i].id,
                                  chain_status);
                }

                operation_status = 0;
                chain_status =
                    M5Chain.setIMUGyroODR(devices_list->devices[i].id, CHAIN_IMU_GYRO_ODR_104HZ, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] set imu gyro odr success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("ID[%d] set imu gyro odr failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                chain_imu_gyro_odr_t gyro_odr = CHAIN_IMU_GYRO_ODR_OFF;
                chain_status                  = M5Chain.getIMUGyroODR(devices_list->devices[i].id, &gyro_odr);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] get imu gyro odr success, odr:%d\r\n", devices_list->devices[i].id, gyro_odr);
                } else {
                    Serial.printf("ID[%d] get imu gyro odr failed, chain_status:%d\r\n", devices_list->devices[i].id,
                                  chain_status);
                }

                operation_status = 0;
                chain_status     = M5Chain.setIMUEventTriggerMode(devices_list->devices[i].id, imu_event_trigger_mode,
                                                                  &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] set imu event trigger mode success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("ID[%d] set imu event trigger mode failed, chain_status:%d  operation_status:%d\r\n ",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                uint8_t event_trigger_mode = 0;
                chain_status = M5Chain.getIMUEventTriggerMode(devices_list->devices[i].id, &event_trigger_mode);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] get imu event trigger mode success, mode:0x%02x\r\n",
                                  devices_list->devices[i].id, event_trigger_mode);
                } else {
                    Serial.printf("ID[%d] get imu event trigger mode failed, chain_status:%d\r\n",
                                  devices_list->devices[i].id, chain_status);
                }

                operation_status = 0;
                chain_status =
                    M5Chain.setIMU6AxisWarningThreshold(devices_list->devices[i].id, imu_accel_warning_threshold,
                                                        imu_gyro_warning_threshold, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] set imu 6axis warning threshold success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf(
                        "ID[%d] set imu 6axis warning threshold failed, chain_status:%d  operation_status:%d \r\n",
                        devices_list->devices[i].id, chain_status, operation_status);
                }

                uint8_t accel_warning_threshold = 0;
                uint8_t gyro_warning_threshold  = 0;
                chain_status                    = M5Chain.getIMU6AxisWarningThreshold(devices_list->devices[i].id,
                                                                                      &accel_warning_threshold, &gyro_warning_threshold);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] get imu 6axis warning threshold success, accel:%u gyro:%u\r\n",
                                  devices_list->devices[i].id, accel_warning_threshold, gyro_warning_threshold);
                } else {
                    Serial.printf("ID[%d] get imu 6axis warning threshold failed, chain_status:%d\r\n",
                                  devices_list->devices[i].id, chain_status);
                }

                operation_status = 0;
                chain_status     = M5Chain.setIMUTemperatureWarningThreshold(
                        devices_list->devices[i].id, imu_temperature_low_threshold, imu_temperature_high_threshold,
                        &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] set imu temperature warning threshold success\r\n",
                                  devices_list->devices[i].id);
                } else {
                    Serial.printf(
                        "ID[%d] set imu temperature warning threshold failed, chain_status:%d  operation_status:%d "
                        "\r\n",
                        devices_list->devices[i].id, chain_status, operation_status);
                }

                uint8_t temperature_low_threshold  = 0;
                uint8_t temperature_high_threshold = 0;
                chain_status                       = M5Chain.getIMUTemperatureWarningThreshold(
                                          devices_list->devices[i].id, &temperature_low_threshold, &temperature_high_threshold);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] get imu temperature warning threshold success, low:%u C high:%u C\r\n",
                                  devices_list->devices[i].id, temperature_low_threshold, temperature_high_threshold);
                } else {
                    Serial.printf("ID[%d] get imu temperature warning threshold failed, chain_status:%d\r\n",
                                  devices_list->devices[i].id, chain_status);
                }

                uint8_t event_status = 0;
                uint8_t report_code  = 0;
                if (M5Chain.getIMUEventTrigger(devices_list->devices[i].id, &event_status, &report_code)) {
                    Serial.printf("ID[%d] get imu event trigger success, report_code:0x%02x event_status:0x%02x\r\n",
                                  devices_list->devices[i].id, report_code, event_status);
                } else {
                    Serial.printf("ID[%d] get imu event trigger no record\r\n", devices_list->devices[i].id);
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
        if (devices_list->devices[i].device_type != CHAIN_IMU_TYPE_CODE) {
            continue;
        }

        uint8_t id = devices_list->devices[i].id;

        chain_status = M5Chain.getIMUAccelData(id, &accel_x, &accel_y, &accel_z);
        if (chain_status == CHAIN_OK) {
            Serial.printf("ID[%d] accel: x:%d mg y:%d mg z:%d mg\r\n", id, accel_x, accel_y, accel_z);
        } else {
            Serial.printf("ID[%d] get imu accel data failed, chain_status:%d\r\n", id, chain_status);
        }

        chain_status = M5Chain.getIMUGyroData(id, &gyro_x, &gyro_y, &gyro_z);
        if (chain_status == CHAIN_OK) {
            Serial.printf("ID[%d] gyro: x:%d dps y:%d dps z:%d dps\r\n", id, gyro_x, gyro_y, gyro_z);
        } else {
            Serial.printf("ID[%d] get imu gyro data failed, chain_status:%d\r\n", id, chain_status);
        }

        chain_status = M5Chain.getIMUAccelRawData(id, &accel_raw_x, &accel_raw_y, &accel_raw_z);
        if (chain_status == CHAIN_OK) {
            Serial.printf("ID[%d] accel raw: x:%d y:%d z:%d\r\n", id, accel_raw_x, accel_raw_y, accel_raw_z);
        } else {
            Serial.printf("ID[%d] get imu accel raw data failed, chain_status:%d\r\n", id, chain_status);
        }

        chain_status = M5Chain.getIMUGyroRawData(id, &gyro_raw_x, &gyro_raw_y, &gyro_raw_z);
        if (chain_status == CHAIN_OK) {
            Serial.printf("ID[%d] gyro raw: x:%d y:%d z:%d\r\n", id, gyro_raw_x, gyro_raw_y, gyro_raw_z);
        } else {
            Serial.printf("ID[%d] get imu gyro raw data failed, chain_status:%d\r\n", id, chain_status);
        }

        chain_status = M5Chain.getIMUTemperature(id, &imu_temperature_x100);
        if (chain_status == CHAIN_OK) {
            Serial.printf("ID[%d] imu temperature: %.2f C\r\n", id, imu_temperature_x100 / 100.0f);
        } else {
            Serial.printf("ID[%d] get imu temperature failed, chain_status:%d\r\n", id, chain_status);
        }

        uint8_t event_status = 0;
        uint8_t report_code  = 0;
        while (M5Chain.getIMUEventTrigger(id, &event_status, &report_code, 100)) {
            uint16_t event_trigger_res = ((uint16_t)report_code << 8) | event_status;

            if ((event_trigger_res & CHAIN_IMU_EVENT_TRIGGER_REPORT_MASK) != CHAIN_IMU_EVENT_TRIGGER_REPORT_TYPE) {
                // Serial.printf("IMU ID[%d] event trigger report type mismatch, report_code:0x%02x
                // event_status:0x%02x\r\n",
                //               id, report_code, event_status);
                continue;
            }

            uint8_t changed_status = event_status ^ imu_event_trigger_last_status[id];
            if (!imu_event_trigger_last_valid[id] || imu_event_trigger_last_report_code[id] != report_code) {
                changed_status = event_status;
            }

            imu_event_trigger_last_valid[id]       = true;
            imu_event_trigger_last_status[id]      = event_status;
            imu_event_trigger_last_report_code[id] = report_code;

            if (changed_status == 0) {
                continue;
            }

            if (changed_status & CHAIN_IMU_EVENT_STATUS_ACCEL_MASK) {
                switch (event_trigger_res & CHAIN_IMU_EVENT_TRIGGER_ACCEL_MASK) {
                    case CHAIN_IMU_EVENT_TRIGGER_ACCEL_ACTIVE:
                        Serial.printf("IMU ID[%d] accel threshold trigger result is: active\r\n", id);
                        break;
                    case CHAIN_IMU_EVENT_TRIGGER_ACCEL_RELEASE:
                        Serial.printf("IMU ID[%d] accel threshold trigger result is: release\r\n", id);
                        break;
                    default:
                        Serial.printf("IMU ID[%d] accel threshold trigger result is: unknown, event_status:0x%02x\r\n",
                                      id, event_status);
                        break;
                }
            }

            if (changed_status & CHAIN_IMU_EVENT_STATUS_GYRO_MASK) {
                switch (event_trigger_res & CHAIN_IMU_EVENT_TRIGGER_GYRO_MASK) {
                    case CHAIN_IMU_EVENT_TRIGGER_GYRO_ACTIVE:
                        Serial.printf("IMU ID[%d] gyro threshold trigger result is: active\r\n", id);
                        break;
                    case CHAIN_IMU_EVENT_TRIGGER_GYRO_RELEASE:
                        Serial.printf("IMU ID[%d] gyro threshold trigger result is: release\r\n", id);
                        break;
                    default:
                        Serial.printf("IMU ID[%d] gyro threshold trigger result is: unknown, event_status:0x%02x\r\n",
                                      id, event_status);
                        break;
                }
            }

            if (changed_status & CHAIN_IMU_EVENT_STATUS_TEMPERATURE_MASK) {
                switch (event_trigger_res & CHAIN_IMU_EVENT_TRIGGER_TEMPERATURE_MASK) {
                    case CHAIN_IMU_EVENT_TRIGGER_TEMPERATURE_HIGH_RELEASE:
                        Serial.printf("IMU ID[%d] temperature trigger result is: high release\r\n", id);
                        break;
                    case CHAIN_IMU_EVENT_TRIGGER_TEMPERATURE_HIGH_ACTIVE:
                        Serial.printf("IMU ID[%d] temperature trigger result is: high active\r\n", id);
                        break;
                    case CHAIN_IMU_EVENT_TRIGGER_TEMPERATURE_LOW_ACTIVE:
                        Serial.printf("IMU ID[%d] temperature trigger result is: low active\r\n", id);
                        break;
                    case CHAIN_IMU_EVENT_TRIGGER_TEMPERATURE_LOW_RELEASE:
                        Serial.printf("IMU ID[%d] temperature trigger result is: low release\r\n", id);
                        break;
                    default:
                        Serial.printf("IMU ID[%d] temperature trigger result is: unknown, event_status:0x%02x\r\n", id,
                                      event_status);
                        break;
                }
            }
        }
    }

    delay(500);
}

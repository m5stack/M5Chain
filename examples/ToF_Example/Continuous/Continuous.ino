/*
 *SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#include "M5Chain.h"

#define TXD_PIN GPIO_NUM_21  // Tx
#define RXD_PIN GPIO_NUM_22  // Rx

Chain M5Chain;

device_list_t *devices_list;

uint16_t device_nums        = 0;
uint8_t operation_status    = 0;
chain_status_t chain_status = CHAIN_OK;

uint8_t rgb_test[5][3] = {
    {0xFF, 0x00, 0x00}, {0x00, 0xFF, 0x00}, {0x00, 0x00, 0xFF}, {0xFF, 0xFF, 0xFF}, {0x00, 0x00, 0x00},
};

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
    Serial.println("M5Chain ToF Test");
    M5Chain.begin(&Serial2, 115200, RXD_PIN, TXD_PIN);

    if (M5Chain.isDeviceConnected()) {
        Serial.println("devices is connected");
        chain_status = M5Chain.getDeviceNum(&device_nums);
        if (chain_status == CHAIN_OK) {
            devices_list          = (device_list_t *)malloc(sizeof(device_list_t));
            devices_list->count   = device_nums;
            devices_list->devices = (device_info_t *)malloc(sizeof(device_info_t) * device_nums);
            if (M5Chain.getDeviceList(devices_list)) {
                Serial.println("devices list get success");
                printDeviceList(devices_list);
            } else {
                Serial.println("devices list get failed");
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
            if (devices_list->devices[i].device_type == CHAIN_TOF_TYPE_CODE) {
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

                // 设置测量模式
                chain_status = M5Chain.setToFMeasureMode(devices_list->devices[i].id, CHAIN_TOF_MODE_CONTINUOUS,
                                                         &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("TOF ID[%d] set measure mode CONTINUOUS success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf(
                        "TOF ID[%d] set measure mode CONTINUOUS failed, chain_status:%d  operation_status:%d \r\n",
                        devices_list->devices[i].id, chain_status, operation_status);
                }

                // 设置测量时间
                uint8_t measurement_time = 33;  // ms
                chain_status =
                    M5Chain.setToFMeasureTime(devices_list->devices[i].id, measurement_time, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("TOF ID[%d] set measurement_time %d success\r\n", devices_list->devices[i].id,
                                  measurement_time);
                } else {
                    Serial.printf(
                        "TOF ID[%d] set measurement_time %d failed, chain_status:%d  operation_status:%d \r\n",
                        devices_list->devices[i].id, measurement_time, chain_status, operation_status);
                }
            }
        }
    } else {
        Serial.println("devices list is NULL");
    }
}

void loop()
{
    if (devices_list) {
        for (uint8_t i = 0; i < devices_list->count; i++) {
            if (devices_list->devices[i].device_type == CHAIN_TOF_TYPE_CODE) {
                uint16_t distance        = 0;
                uint8_t measurement_time = 0;
                chain_tof_mode_t mode;
                chain_tof_measure_status_t measure_status;
                chain_status = M5Chain.getToFDistance(devices_list->devices[i].id, &distance);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("TOF ID[%d] distance:%d \r\n", devices_list->devices[i].id, distance);
                } else {
                    Serial.printf("TOF ID[%d] get distance failed, chain_status:%d \r\n", devices_list->devices[i].id,
                                  chain_status);
                }
                chain_status = M5Chain.getToFMeasureTime(devices_list->devices[i].id, &measurement_time);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("TOF ID[%d] measurement_time:%d \r\n", devices_list->devices[i].id, measurement_time);
                } else {
                    Serial.printf("TOF ID[%d] get measurement_time failed, chain_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status);
                }
                chain_status = M5Chain.getToFMeasureMode(devices_list->devices[i].id, &mode);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("TOF ID[%d] mode:%d \r\n", devices_list->devices[i].id, mode);
                } else {
                    Serial.printf("TOF ID[%d] get mode failed, chain_status:%d \r\n", devices_list->devices[i].id,
                                  chain_status);
                }
                chain_status = M5Chain.getToFMeasureStatus(devices_list->devices[i].id, &measure_status);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("TOF ID[%d] measure_status:%d \r\n", devices_list->devices[i].id, measure_status);
                } else {
                    Serial.printf("TOF ID[%d] get measure_status failed, chain_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status);
                }
            }
        }
    }
    delay(100);
}
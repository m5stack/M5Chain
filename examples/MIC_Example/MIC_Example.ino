/*
 *SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
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
    Serial.println("M5Chain MIC Test");
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
            if (devices_list->devices[i].device_type == CHAIN_MIC_TYPE_CODE) {
                chain_status = M5Chain.setRGBLight(devices_list->devices[i].id, 100, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("MIC ID[%d] set rgb light success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("MIC ID[%d] set rgb light failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }
                for (uint8_t j = 0; j < 5; j++) {
                    uint8_t rgb[3] = {0};
                    chain_status =
                        M5Chain.setRGBValue(devices_list->devices[i].id, 0, 1, rgb_test[j], 3, &operation_status);
                    if (chain_status == CHAIN_OK && operation_status) {
                        Serial.printf("MIC ID[%d] set rgb %d %d %d success\r\n", devices_list->devices[i].id,
                                      rgb_test[j][0], rgb_test[j][1], rgb_test[j][2]);
                    } else {
                        Serial.printf("MIC ID[%d] set rgb %d %d %d failed, chain_status:%d  operation_status:%d \r\n",
                                      devices_list->devices[i].id, rgb_test[j][0], rgb_test[j][1], rgb_test[j][2],
                                      chain_status, operation_status);
                    }
                    chain_status = M5Chain.getRGBValue(devices_list->devices[i].id, 0, 1, rgb, 3, &operation_status);
                    if (chain_status == CHAIN_OK && operation_status) {
                        Serial.printf("MIC ID[%d] get rgb %d %d %d success \r\n", devices_list->devices[i].id, rgb[0],
                                      rgb[1], rgb[2]);
                    } else {
                        Serial.printf("MIC ID[%d] get rgb %d %d %d failed, chain_status:%d  operation_status:%d \r\n",
                                      devices_list->devices[i].id, rgb[0], rgb[1], rgb[2], chain_status,
                                      operation_status);
                    }
                    delay(500);
                }

                chain_status = M5Chain.setMICThresholdValue(devices_list->devices[i].id, 500, &operation_status,
                                                            CHAIN_SAVE_FLASH_DISABLE);
                //  chain_status = M5Chain.setMICThresholdValue(devices_list->devices[i].id, 500,
                //  &operation_status,CHAIN_SAVE_FLASH_ENABLE);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("MIC ID[%d] set mic threshold value success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf(
                        "MIC ID[%d] set mic threshold value failed, chain_status:%d  operation_status:%d \r\n",
                        devices_list->devices[i].id, chain_status, operation_status);
                }

                chain_status =
                    M5Chain.setMICReportMode(devices_list->devices[i].id, CHAIN_MIC_REPORT_MODE, &operation_status);
                // chain_status = M5Chain.setMICReportMode(devices_list->devices[i].id, CHAIN_MIC_NONE_REPORT_MODE,
                // &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("MIC ID[%d] set mic mode success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("MIC ID[%d] set mic mode failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                chain_status = M5Chain.setMICTriggerInterval(devices_list->devices[i].id, 300, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("MIC ID[%d] set mic trigger interval success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf(
                        "MIC ID[%d] set mic trigger interval failed, chain_status:%d  operation_status:%d \r\n",
                        devices_list->devices[i].id, chain_status, operation_status);
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
            if (devices_list->devices[i].device_type == CHAIN_MIC_TYPE_CODE) {
                uint16_t mic_12bit_adc = 0;
                uint8_t mic_8bit_adc   = 0;
                uint16_t mic_threshold = 0;
                chain_mic_report_mode_t mic_report_mode;
                uint16_t mic_trigger_cycle = 0;
                chain_mic_trigger_t trigger_status;
                chain_status = M5Chain.getMIC12BitAdc(devices_list->devices[i].id, &mic_12bit_adc);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("MIC ID[%d] mic 12bit adc value: %d \r\n", devices_list->devices[i].id,
                                  mic_12bit_adc);
                }
                chain_status = M5Chain.getMIC8BitAdc(devices_list->devices[i].id, &mic_8bit_adc);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("MIC ID[%d] mic 8bit adc value: %d \r\n", devices_list->devices[i].id, mic_8bit_adc);
                }
                chain_status = M5Chain.getMICThresholdValue(devices_list->devices[i].id, &mic_threshold);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("MIC ID[%d] mic threshold value: %d \r\n", devices_list->devices[i].id,
                                  mic_threshold);
                }
                chain_status = M5Chain.getMICReportMode(devices_list->devices[i].id, &mic_report_mode);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("MIC ID[%d] mic report mode: %d \r\n", devices_list->devices[i].id, mic_report_mode);
                }
                chain_status = M5Chain.getMICTriggerInterval(devices_list->devices[i].id, &mic_trigger_cycle);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("MIC ID[%d] mic trigger interval: %d \r\n", devices_list->devices[i].id,
                                  mic_trigger_cycle);
                }
                while (M5Chain.getMICTriggerStatus(devices_list->devices[i].id, &trigger_status)) {
                    switch (trigger_status) {
                        case CHAIN_MIC_LOW_THRESHOLD_TRIGGER:
                            Serial.printf("MIC ID[%d] CHAIN_MIC_LOW_THRESHOLD_TRIGGER\r\n",
                                          devices_list->devices[i].id);
                            break;
                        case CHAIN_MIC_HIGH_THRESHOLD_TRIGGER:
                            Serial.printf("MIC ID[%d] CHAIN_MIC_HIGH_THRESHOLD_TRIGGER\r\n",
                                          devices_list->devices[i].id);
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }
    delay(100);
}

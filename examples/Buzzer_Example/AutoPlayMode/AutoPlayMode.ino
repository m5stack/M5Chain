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
    Serial.println("M5Chain Buzzer Test");
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
            if (devices_list->devices[i].device_type == CHAIN_BUZZER_TYPE_CODE) {
                chain_status = M5Chain.setRGBLight(devices_list->devices[i].id, 100, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("Buzzer ID[%d] set rgb light success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("Buzzer ID[%d] set rgb light failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }
                for (uint8_t j = 0; j < 5; j++) {
                    uint8_t rgb[3] = {0};
                    chain_status =
                        M5Chain.setRGBValue(devices_list->devices[i].id, 0, 1, rgb_test[j], 3, &operation_status);
                    if (chain_status == CHAIN_OK && operation_status) {
                        Serial.printf("Buzzer ID[%d] set rgb %d %d %d success\r\n", devices_list->devices[i].id,
                                      rgb_test[j][0], rgb_test[j][1], rgb_test[j][2]);
                    } else {
                        Serial.printf(
                            "Buzzer ID[%d] set rgb %d %d %d failed, chain_status:%d  operation_status:%d \r\n",
                            devices_list->devices[i].id, rgb_test[j][0], rgb_test[j][1], rgb_test[j][2], chain_status,
                            operation_status);
                    }
                    chain_status = M5Chain.getRGBValue(devices_list->devices[i].id, 0, 1, rgb, 3, &operation_status);
                    if (chain_status == CHAIN_OK && operation_status) {
                        Serial.printf("Buzzer ID[%d] get rgb %d %d %d success \r\n", devices_list->devices[i].id,
                                      rgb[0], rgb[1], rgb[2]);
                    } else {
                        Serial.printf(
                            "Buzzer ID[%d] get rgb %d %d %d failed, chain_status:%d  operation_status:%d \r\n",
                            devices_list->devices[i].id, rgb[0], rgb[1], rgb[2], chain_status, operation_status);
                    }
                    delay(500);
                }

                chain_status =
                    M5Chain.setBuzzerMode(devices_list->devices[i].id, BUZZER_MODE_AUTO_PLAY, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("Buzzer ID[%d] set mode success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("Buzzer ID[%d] set mode failed, chain_status:%d  operation_status:%d \r\n",
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
    uint16_t randFreq = random(500, 4500);
    uint8_t randDuty  = random(10, 90);
    uint16_t duration = 600;

    if (devices_list) {
        for (uint8_t i = 0; i < devices_list->count; i++) {
            if (devices_list->devices[i].device_type == CHAIN_BUZZER_TYPE_CODE) {
                chain_status = M5Chain.setBuzzerAutoPlay(devices_list->devices[i].id, randFreq, randDuty, duration,
                                                         &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("Buzzer ID[%d] set auto play success random Freq: %d Hz, Duty: %d %%\r\n",
                                  devices_list->devices[i].id, randFreq, randDuty);
                } else {
                    Serial.printf("Buzzer ID[%d] set buzzer auto play failed, chain_status:%d  operation_status:%d\r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }
            }
        }
    }

    delay(1000);
}

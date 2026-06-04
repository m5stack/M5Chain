
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
    Serial.println("M5Chain Mono Test");
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
            if (devices_list->devices[i].device_type == CHAIN_MONO_TYPE_CODE) {
                chain_status = M5Chain.setMonoMode(devices_list->devices[i].id, MONO_PIXEL_MODE, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("MONO ID[%d] set pixel mode success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("MONO ID[%d] set pixel mode failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }
                chain_status = M5Chain.setMonoRotation(devices_list->devices[i].id, MONO_ROTATION_0, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("MONO ID[%d] set rotation success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("MONO ID[%d] set rotation failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                chain_status =
                    M5Chain.setMonoBrightness(devices_list->devices[i].id, MONO_BRIGHTNESS_LEVEL_7, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("MONO ID[%d] set brightness success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("MONO ID[%d] set brightness failed, chain_status:%d  operation_status:%d \r\n",
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
            if (devices_list->devices[i].device_type == CHAIN_MONO_TYPE_CODE) {
                mono_mode_t mono_mode;
                chain_status = M5Chain.getMonoMode(devices_list->devices[i].id, &mono_mode);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("MONO ID[%d] mono mode:%d\r\n", devices_list->devices[i].id, mono_mode);
                } else {
                    Serial.printf("MONO ID[%d] get mode failed, chain_status:%d \r\n", devices_list->devices[i].id,
                                  chain_status);
                }

                chain_status = M5Chain.setMonoClear(devices_list->devices[i].id, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("MONO ID[%d] set clear success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("MONO ID[%d] set clear failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                for (uint8_t j = 0; j < 8; j++) {
                    for (uint8_t k = 0; k < 8; k++) {
                        chain_status = M5Chain.setMonoPixel(devices_list->devices[i].id, k, j, true, &operation_status);
                        if (chain_status == CHAIN_OK && operation_status == 1) {
                            Serial.printf("MONO ID[%d] set pixel success, x:%d, y:%d\r\n", devices_list->devices[i].id,
                                          j, k);
                        } else {
                            Serial.printf("MONO ID[%d] set pixel failed, chain_status:%d  operation_status:%d \r\n",
                                          devices_list->devices[i].id, chain_status, operation_status);
                        }
                        delay(50);
                    }
                }
                delay(100);
                chain_status = M5Chain.setMonoClear(devices_list->devices[i].id, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("MONO ID[%d] set clear success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("MONO ID[%d] set clear failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                MonoPixelInfo pattern_checkerboard[] = {
                    {0, 0, true},  {1, 0, false}, {2, 0, true},  {3, 0, false}, {4, 0, true},  {5, 0, false},
                    {6, 0, true},  {7, 0, false}, {0, 1, false}, {1, 1, true},  {2, 1, false}, {3, 1, true},
                    {4, 1, false}, {5, 1, true},  {6, 1, false}, {7, 1, true},  {0, 2, true},  {1, 2, false},
                    {2, 2, true},  {3, 2, false}, {4, 2, true},  {5, 2, false}, {6, 2, true},  {7, 2, false},
                    {0, 3, false}, {1, 3, true},  {2, 3, false}, {3, 3, true},  {4, 3, false}, {5, 3, true},
                    {6, 3, false}, {7, 3, true},  {0, 4, true},  {1, 4, false}, {2, 4, true},  {3, 4, false},
                    {4, 4, true},  {5, 4, false}, {6, 4, true},  {7, 4, false}, {0, 5, false}, {1, 5, true},
                    {2, 5, false}, {3, 5, true},  {4, 5, false}, {5, 5, true},  {6, 5, false}, {7, 5, true},
                    {0, 6, true},  {1, 6, false}, {2, 6, true},  {3, 6, false}, {4, 6, true},  {5, 6, false},
                    {6, 6, true},  {7, 6, false}, {0, 7, false}, {1, 7, true},  {2, 7, false}, {3, 7, true},
                    {4, 7, false}, {5, 7, true},  {6, 7, false}, {7, 7, true}};
                chain_status = M5Chain.setMonoPixel(devices_list->devices[i].id, pattern_checkerboard,
                                                    sizeof(pattern_checkerboard) / sizeof(pattern_checkerboard[0]),
                                                    &operation_status);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("MONO ID[%d] set pixels success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("MONO ID[%d] set pixels failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }
                delay(1000);
                chain_status = M5Chain.setMonoClear(devices_list->devices[i].id, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("MONO ID[%d] set clear success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("MONO ID[%d] set clear failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                for (char c = 'A'; c <= 'Z'; ++c) {
                    chain_status = M5Chain.setMonoPrintChar(devices_list->devices[i].id, c, 1, 1, &operation_status);
                    if (chain_status == CHAIN_OK && operation_status == 1) {
                        Serial.printf("MONO ID[%d] set char success, ascii:%c, x:%d, y:%d\r\n",
                                      devices_list->devices[i].id, c, 1, 1);
                    } else {
                        Serial.printf("MONO ID[%d] set char failed, chain_status:%d  operation_status:%d \r\n",
                                      devices_list->devices[i].id, chain_status, operation_status);
                    }
                    delay(200);
                }

                delay(1000);
                chain_status = M5Chain.setMonoClear(devices_list->devices[i].id, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("MONO ID[%d] set clear success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("MONO ID[%d] set clear failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                uint8_t pattern_arrow[8] = {
                    0b00011000,  // Y0
                    0b00111100,  // Y1
                    0b01111110,  // Y2
                    0b11111111,  // Y3
                    0b00111100,  // Y4
                    0b00111100,  // Y5
                    0b00111100,  // Y6
                    0b00111100   // Y7
                };

                chain_status =
                    M5Chain.setMonoBufferRefresh(devices_list->devices[i].id, pattern_arrow, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("MONO ID[%d] set buffer refresh success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("MONO ID[%d] set buffer refresh failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                for (uint8_t j = 0; j <= (MONO_ROTATION_270 + 1); j++) {
                    chain_status = M5Chain.setMonoRotation(
                        devices_list->devices[i].id, (mono_rotation_t)(j % (MONO_ROTATION_270 + 1)), &operation_status);
                    if (chain_status == CHAIN_OK && operation_status == 1) {
                        Serial.printf("MONO ID[%d] set rotation success\r\n", devices_list->devices[i].id);
                    } else {
                        Serial.printf("MONO ID[%d] set rotation failed, chain_status:%d  operation_status:%d \r\n",
                                      devices_list->devices[i].id, chain_status, operation_status);
                    }
                    delay(1000);
                }

                for (uint8_t j = 1; j <= MONO_BRIGHTNESS_LEVEL_7; j++) {
                    chain_status = M5Chain.setMonoBrightness(devices_list->devices[i].id, (mono_brightness_level_t)j,
                                                             &operation_status);
                    if (chain_status == CHAIN_OK && operation_status == 1) {
                        Serial.printf("MONO ID[%d] set brightness success\r\n", devices_list->devices[i].id);
                    } else {
                        Serial.printf("MONO ID[%d] set brightness failed, chain_status:%d  operation_status:%d \r\n",
                                      devices_list->devices[i].id, chain_status, operation_status);
                    }
                    delay(1000);
                }

                chain_status = M5Chain.setMonoClear(devices_list->devices[i].id, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("MONO ID[%d] set clear success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("MONO ID[%d] set clear failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }
            }
        }
    }
    delay(100);
}
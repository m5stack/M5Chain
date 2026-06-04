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
                chain_status =
                    M5Chain.setMonoMode(devices_list->devices[i].id, MONO_STRING_SCROLL_MODE, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("MONO ID[%d] set string scroll mode success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf(
                        "MONO ID[%d] set string scroll mode failed, chain_status:%d  operation_status:%d \r\n",
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

                chain_status =
                    M5Chain.setMonoStringScroll(devices_list->devices[i].id, "Hello World!  ", MONO_SCROLL_LEFT,
                                                MONO_SCROLL_MODE_LOOP, 100, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("MONO ID[%d] set string scroll success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("MONO ID[%d] set string scroll failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }
            }
        }
    }

    while (1) {
        delay(1000);
    }
}
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
    Serial.println("M5Chain RGB Test");
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
            if (devices_list->devices[i].device_type == CHAIN_RGB_TYPE_CODE) {
                chain_status =
                    M5Chain.setRGBMode(devices_list->devices[i].id, RGB_STRING_SCROLL_MODE, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("RGB ID[%d] set string scroll mode success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("RGB ID[%d] set string scroll mode failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }
                chain_status = M5Chain.setRGBRotation(devices_list->devices[i].id, RGB_ROTATION_0, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("RGB ID[%d] set rotation success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("RGB ID[%d] set rotation failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                chain_status = M5Chain.setRGBBrightness(devices_list->devices[i].id, 30, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("RGB ID[%d] set brightness success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("RGB ID[%d] set brightness failed, chain_status:%d  operation_status:%d \r\n",
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
            if (devices_list->devices[i].device_type == CHAIN_RGB_TYPE_CODE) {
                rgb_mode_t rgb_mode;
                chain_status = M5Chain.getRGBMode(devices_list->devices[i].id, &rgb_mode);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("RGB ID[%d] rgb mode:%d\r\n", devices_list->devices[i].id, rgb_mode);
                } else {
                    Serial.printf("RGB ID[%d] get mode failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                chain_status =
                    M5Chain.setRGBStringScroll(devices_list->devices[i].id, "Hello World!  ", RGB_SCROLL_LEFT,
                                               RGB_SCROLL_MODE_LOOP, 100, 0x07E0, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("RGB ID[%d] set green string scroll success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf(
                        "RGB ID[%d] set green string scroll failed, chain_status:%d  operation_status:%d \r\n",
                        devices_list->devices[i].id, chain_status, operation_status);
                }
            }
        }
    }

    delay(10000);

    if (devices_list) {
        for (uint8_t i = 0; i < devices_list->count; i++) {
            if (devices_list->devices[i].device_type == CHAIN_RGB_TYPE_CODE) {
                chain_status =
                    M5Chain.setRGBStringScroll(devices_list->devices[i].id, "Hello World!  ", RGB_SCROLL_LEFT,
                                               RGB_SCROLL_MODE_LOOP, 100, RGB_SCROLL_COLOR_GRADIENT, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("RGB ID[%d] set gradient string scroll success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf(
                        "RGB ID[%d] set gradient string scroll failed, chain_status:%d  operation_status:%d \r\n",
                        devices_list->devices[i].id, chain_status, operation_status);
                }
            }
        }
    }

    delay(10000);
}
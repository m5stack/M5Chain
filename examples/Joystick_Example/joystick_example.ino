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
    Serial.println("M5Chain Joystick Test");
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
            if (devices_list->devices[i].device_type == CHAIN_JOYSTICK_TYPE_CODE) {
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
                chain_status =
                    M5Chain.setKeyButtonTriggerInterval(devices_list->devices[i].id, BUTTON_DOUBLE_CLICK_TIME_200MS,
                                                        BUTTON_LONG_PRESS_TIME_3S, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("JOYSTICK ID[%d] set key button trigger interval success\r\n",
                                  devices_list->devices[i].id);
                } else {
                    Serial.printf(
                        "JOYSTICK ID[%d] set key button trigger interval failed, chain_status:%d  operation_status:%d "
                        "\r\n",
                        devices_list->devices[i].id, chain_status, operation_status);
                }
                chain_status =
                    M5Chain.setKeyButtonMode(devices_list->devices[i].id, CHAIN_BUTTON_REPORT_MODE, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("JOYSTICK ID[%d] set key button mode success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf(
                        "JOYSTICK ID[%d] set key button mode failed, chain_status:%d  operation_status:%d \r\n",
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
            if (devices_list->devices[i].device_type == CHAIN_JOYSTICK_TYPE_CODE) {
                uint16_t xAdcValue = 0;
                uint16_t yAdcValue = 0;
                uint8_t xAdcValue8 = 0;
                uint8_t yAdcValue8 = 0;
                uint16_t mapRange[8];
                int16_t xMapAdcValue  = 0;
                int16_t yMapAdcValue  = 0;
                int8_t xMapAdcValue8  = 0;
                int8_t yMapAdcValue8  = 0;
                uint8_t button_status = 0;
                button_double_click_time_t button_double_click_time;
                button_long_press_time_t button_long_press_time;
                chain_button_press_type_t button_press_type;
                chain_button_mode_t button_mode;
                chain_status = M5Chain.getJoystick16Adc(devices_list->devices[i].id, &xAdcValue, &yAdcValue);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("JOYSTICK ID[%d] xAdcValue:%d, yAdcValue:%d \r\n", devices_list->devices[i].id, xAdcValue, yAdcValue);
                } else {
                    Serial.printf("JOYSTICK ID[%d] get 16 adc value failed, chain_status:%d \r\n", devices_list->devices[i].id, chain_status);
                }
                chain_status = M5Chain.getJoystick8Adc(devices_list->devices[i].id, &xAdcValue8, &yAdcValue8);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("JOYSTICK ID[%d] xAdcValue8:%d, yAdcValue8:%d \r\n", devices_list->devices[i].id, xAdcValue8, yAdcValue8);
                } else {
                    Serial.printf("JOYSTICK ID[%d] get 8 adc value failed, chain_status:%d \r\n", devices_list->devices[i].id, chain_status);
                }

                chain_status = M5Chain.getJoystickMappedRange(devices_list->devices[i].id, mapRange, JOYSTICK_MAP_SIZE);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("JOYSTICK ID[%d] mapRange:%d %d %d %d %d %d %d %d \r\n", devices_list->devices[i].id, mapRange[0], mapRange[1], mapRange[2], mapRange[3], mapRange[4], mapRange[5], mapRange[6], mapRange[7]);
                } else {
                    Serial.printf("JOYSTICK ID[%d] get 16 adc map range failed, chain_status:%d \r\n", devices_list->devices[i].id, chain_status);
                }
                chain_status = M5Chain.getJoystickMappedInt16Value(devices_list->devices[i].id, &xMapAdcValue, &yMapAdcValue);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("JOYSTICK ID[%d] xMapAdcValue:%d, yMapAdcValue:%d \r\n", devices_list->devices[i].id, xMapAdcValue, yMapAdcValue);
                } else {
                    Serial.printf("JOYSTICK ID[%d] get 16 adc map value failed, chain_status:%d \r\n", devices_list->devices[i].id, chain_status);
                }
                chain_status = M5Chain.getJoystickMappedInt8Value(devices_list->devices[i].id, &xMapAdcValue8, &yMapAdcValue8);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("JOYSTICK ID[%d] xMapAdcValue8:%d, yMapAdcValue8:%d \r\n", devices_list->devices[i].id, xMapAdcValue8, yMapAdcValue8);
                } else {
                    Serial.printf("JOYSTICK ID[%d] get 8 adc map value failed, chain_status:%d \r\n", devices_list->devices[i].id, chain_status);
                }

                chain_status = M5Chain.getJoystickButtonStatus(devices_list->devices[i].id, &button_status);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("JOYSTICK ID[%d] button status:%d \r\n", devices_list->devices[i].id, button_status);
                } else {
                    Serial.printf("JOYSTICK ID[%d] get button status failed, chain_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status);
                }
                chain_status = M5Chain.getJoystickButtonTriggerInterval(
                    devices_list->devices[i].id, &button_double_click_time, &button_long_press_time);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("JOYSTICK ID[%d] button double click time:%d, button long press time:%d \r\n",
                                  devices_list->devices[i].id, button_double_click_time, button_long_press_time);
                } else {
                    Serial.printf("JOYSTICK ID[%d] get button trigger interval failed, chain_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status);
                }
                chain_status = M5Chain.getJoystickButtonMode(devices_list->devices[i].id, &button_mode);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("JOYSTICK ID[%d] button mode:%d \r\n", devices_list->devices[i].id, button_mode);
                } else {
                    Serial.printf("JOYSTICK ID[%d] get button mode failed, chain_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status);
                }
                while (M5Chain.getJoystickButtonPressStatus(devices_list->devices[i].id, &button_press_type)) {
                    switch (button_press_type) {
                        case CHAIN_BUTTON_PRESS_SINGLE:
                            Serial.printf("JOYSTICK ID[%d] button press type: single \r\n",
                                          devices_list->devices[i].id);
                            break;
                        case CHAIN_BUTTON_PRESS_DOUBLE:
                            Serial.printf("JOYSTICK ID[%d] button press type: double \r\n",
                                          devices_list->devices[i].id);
                            break;
                        case CHAIN_BUTTON_PRESS_LONG:
                            Serial.printf("JOYSTICK ID[%d] button press type: long \r\n", devices_list->devices[i].id);
                            break;
                    }
                }
            }
        }
    }
    delay(100);
}
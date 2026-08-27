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

uint8_t rgb_test[5][12] = {
    {0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00},
    {0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00},
    {0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
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
    Serial.println("M5Chain Pedal Test");
    M5Chain.begin(&Serial2, 115200, RXD_PIN, TXD_PIN);

    if (M5Chain.isDeviceConnected()) {
        Serial.println("devices is connected");
        chain_status = M5Chain.getDeviceNum(&device_nums);
        if (chain_status == CHAIN_OK) {
            devices_list = (device_list_t *)malloc(sizeof(device_list_t));
            if (devices_list == NULL) {
                Serial.println("allocating device list failed");
                return;
            }
            devices_list->count   = device_nums;
            devices_list->devices = (device_info_t *)malloc(sizeof(device_info_t) * device_nums);
            if (device_nums > 0 && devices_list->devices == NULL) {
                Serial.println("allocating device information failed");
                free(devices_list);
                devices_list = NULL;
                return;
            }
            if (M5Chain.getDeviceList(devices_list)) {
                Serial.println("get devices list success");
                printDeviceList(devices_list);
            } else {
                Serial.println("get devices list failed");
                free(devices_list->devices);
                free(devices_list);
                devices_list = NULL;
                return;
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
            if (devices_list->devices[i].device_type == CHAIN_PEDAL_TYPE_CODE) {
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
                        M5Chain.setRGBValue(devices_list->devices[i].id, 0, 4, rgb_test[j], 12, &operation_status);
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

                chain_status = M5Chain.setPedalWorkMode(devices_list->devices[i].id, CHAIN_PEDAL_SINGLE_MODE,
                                                        &operation_status, CHAIN_SAVE_FLASH_DISABLE);
                // chain_status = M5Chain.setPedalWorkMode(devices_list->devices[i].id, CHAIN_PEDAL_SINGLE_MODE,
                //                                         &operation_status, CHAIN_SAVE_FLASH_ENABLE);
                // chain_status = M5Chain.setPedalWorkMode(devices_list->devices[i].id, CHAIN_PEDAL_SWITCH_MODE,
                //                                         &operation_status, CHAIN_SAVE_FLASH_DISABLE);
                // chain_status = M5Chain.setPedalWorkMode(devices_list->devices[i].id, CHAIN_PEDAL_SWITCH_MODE,
                //                                         &operation_status, CHAIN_SAVE_FLASH_ENABLE);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] set work mode success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("ID[%d] set work mode failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                // chain_status = M5Chain.setPedalButtonMode(devices_list->devices[i].id, CHAIN_BUTTON_NONE_REPORT_MODE,
                //                                           &operation_status);
                chain_status = M5Chain.setPedalButtonMode(devices_list->devices[i].id, CHAIN_BUTTON_REPORT_MODE,
                                                          &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] set button mode success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("ID[%d] set button mode failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                chain_status = M5Chain.setPedalEnterConfigTime(devices_list->devices[i].id, 3, &operation_status,
                                                               CHAIN_SAVE_FLASH_DISABLE);
                // chain_status = M5Chain.setPedalEnterConfigTime(devices_list->devices[i].id, 3, &operation_status,
                //                                                CHAIN_SAVE_FLASH_ENABLE);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] set enter config time success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("ID[%d] set enter config time failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                chain_status = M5Chain.setPedalRGBOverlayValue(devices_list->devices[i].id, 0x0F, &operation_status,
                                                               CHAIN_SAVE_FLASH_DISABLE);
                // chain_status = M5Chain.setPedalRGBOverlayValue(devices_list->devices[i].id, 0, &operation_status,
                //                                                CHAIN_SAVE_FLASH_ENABLE);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] set rgb overlay value success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("ID[%d] set rgb overlay value failed, chain_status:%d  operation_status:%d \r\n",
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
            if (devices_list->devices[i].device_type == CHAIN_PEDAL_TYPE_CODE) {
                uint8_t buttonStatus                         = 0;
                chain_pedal_switch_status_t switchStatus     = CHAIN_PEDAL_SWITCH_OFF_STATUS;
                chain_pedal_work_mode_t pedal_work_mode      = CHAIN_PEDAL_SINGLE_MODE;
                chain_button_mode_t buttonMode               = CHAIN_BUTTON_NONE_REPORT_MODE;
                uint8_t enterConfigTime                      = 0;
                uint8_t rgbOverlayValue                      = 0;
                chain_pedal_press_event_t pedal_press_status = CHAIN_PEDAL_SINGLE_CLICK;
                chain_status = M5Chain.getPedalButtonStatus(devices_list->devices[i].id, &buttonStatus);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("PEDAL ID[%d] get button status %d success \r\n", devices_list->devices[i].id,
                                  buttonStatus);
                } else {
                    Serial.printf(
                        "PEDAL ID[%d] get button status %d failed, chain_status:%d operation_status : %d\r\n ",
                        devices_list->devices[i].id, buttonStatus, chain_status, operation_status);
                }
                chain_status = M5Chain.getPedalSwitchStatus(devices_list->devices[i].id, &switchStatus);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("PEDAL ID[%d] get switch status %d success \r\n", devices_list->devices[i].id,
                                  switchStatus);
                } else {
                    Serial.printf(
                        "PEDAL ID[%d] get switch status %d failed, chain_status:%d operation_status : %d\r\n ",
                        devices_list->devices[i].id, switchStatus, chain_status, operation_status);
                }
                chain_status = M5Chain.getPedalWorkMode(devices_list->devices[i].id, &pedal_work_mode);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("PEDAL ID[%d] get work mode %d success \r\n", devices_list->devices[i].id,
                                  pedal_work_mode);
                } else {
                    Serial.printf("PEDAL ID[%d] get work mode %d failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, pedal_work_mode, chain_status, operation_status);
                }

                chain_status = M5Chain.getPedalButtonMode(devices_list->devices[i].id, &buttonMode);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("PEDAL ID[%d] get button mode %d success \r\n", devices_list->devices[i].id,
                                  buttonMode);
                } else {
                    Serial.printf("PEDAL ID[%d] get button mode %d failed, chain_status:%d  operation_status:%d\r\n",
                                  devices_list->devices[i].id, buttonMode, chain_status, operation_status);
                }

                chain_status = M5Chain.getPedalEnterConfigTime(devices_list->devices[i].id, &enterConfigTime);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("PEDAL ID[%d] get enter config time %d success \r\n", devices_list->devices[i].id,
                                  enterConfigTime);
                } else {
                    Serial.printf(
                        "PEDAL ID[%d] get enter config time %d failed, chain_status:%d  operation_status:%d \r\n",
                        devices_list->devices[i].id, enterConfigTime, chain_status, operation_status);
                }

                chain_status = M5Chain.getPedalRGBOverlayValue(devices_list->devices[i].id, &rgbOverlayValue);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("PEDAL ID[%d] get rgb overlay value %d success \r\n", devices_list->devices[i].id,
                                  rgbOverlayValue);
                } else {
                    Serial.printf(
                        "PEDAL ID[%d] get rgb overlay value %d failed, chain_status:%d  operation_status:%d \r\n",
                        devices_list->devices[i].id, rgbOverlayValue, chain_status, operation_status);
                }

                while (M5Chain.getPedalTriggerStatus(devices_list->devices[i].id, &pedal_press_status)) {
                    switch (pedal_press_status) {
                        case CHAIN_PEDAL_SINGLE_CLICK:
                            Serial.printf("PEDAL ID[%d] get single click success \r\n", devices_list->devices[i].id);
                            break;
                        case CHAIN_PEDAL_SWITCH_OFF:
                            Serial.printf("PEDAL ID[%d] get switch off success \r\n", devices_list->devices[i].id);
                            break;
                        case CHAIN_PEDAL_SWITCH_ON:
                            Serial.printf("PEDAL ID[%d] get switch on success \r\n", devices_list->devices[i].id);
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }
    if (M5Chain.getEnumPleaseNum()) {
        Serial.println("get enum please num");
    }
    delay(100);
}

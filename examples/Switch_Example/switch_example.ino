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
uint16_t open_threshold     = 3967;
uint16_t close_threshold    = 128;

uint8_t rgb_test[5][3] = {
    {0xFF, 0x00, 0x00}, {0x00, 0xFF, 0x00}, {0x00, 0x00, 0xFF}, {0xFF, 0xFF, 0xFF}, {0x00, 0x00, 0x00},
};

uint8_t uid32[4]  = {0};
uint8_t uid96[12] = {0};
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
    Serial.println("M5Chain Switch Test");
    M5Chain.begin(&Serial2, 115200, RXD_PIN, TXD_PIN);

    if (M5Chain.isDeviceConnected()) {
        Serial.println("devices is connected");
        chain_status = M5Chain.getDeviceNum(&device_nums);
        if (chain_status == CHAIN_OK) {
            devices_list = (device_list_t *)malloc(sizeof(device_list_t));
            if (devices_list == NULL) {
                Serial.println("devices list malloc failed");
                return;
            }
            devices_list->count   = device_nums;
            devices_list->devices = (device_info_t *)malloc(sizeof(device_info_t) * device_nums);
            if (devices_list->devices == NULL) {
                Serial.println("devices info malloc failed");
                free(devices_list);
                devices_list = NULL;
                return;
            }
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
            if (devices_list->devices[i].device_type == CHAIN_SWITCH_TYPE_CODE) {
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

                operation_status = 0;
                chain_status =
                    M5Chain.setSwitchThresholdValue(devices_list->devices[i].id, open_threshold, close_threshold,
                                                    &operation_status, CHAIN_SAVE_FLASH_DISABLE);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] set switch threshold success, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                } else {
                    Serial.printf("ID[%d] set switch threshold failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                uint16_t open_threshold, close_threshold = 0;
                chain_status =
                    M5Chain.getSwitchThresholdValue(devices_list->devices[i].id, &open_threshold, &close_threshold);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] set switch threshold success, open_threshold:%d, close_threshold:%d\r\n",
                                  devices_list->devices[i].id, open_threshold, close_threshold);
                } else {
                    Serial.printf("ID[%d] set switch threshold failed, chain_status:%d\r\n",
                                  devices_list->devices[i].id, chain_status);
                }

                chain_status = M5Chain.setSwitchAutoTriggerMode(devices_list->devices[i].id, CHAIN_SLIP_REPORT_MODE,
                                                                &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf(
                        "ID[%d] set switch auto trigger mode success, chain_status:%d  operation_status:%d \r\n ",
                        devices_list->devices[i].id, chain_status, operation_status);
                } else {
                    Serial.printf(
                        "ID[%d] set switch auto trigger mode failed, chain_status:%d  operation_status:%d \r\n ",
                        devices_list->devices[i].id, chain_status, operation_status);
                }

                chain_status = M5Chain.setSwitchDirection(
                    devices_list->devices[i].id, CHAIN_SWITCH_DOWNUP_INC, &operation_status,
                    CHAIN_SAVE_FLASH_DISABLE);  // Switch slip direction increasing, not save to flash

                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf(
                        "Switch ID[%d] set switch slip direction success, chain_status:%d operation_status : %d\r\n",
                        devices_list->devices[i].id, chain_status, operation_status);
                } else {
                    Serial.printf(
                        "Switch ID[%d] set switch slip direction failed, chain_status:%d  operation_status:%d \r\n",
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
            if (devices_list->devices[i].device_type == CHAIN_SWITCH_TYPE_CODE) {
                uint16_t switch12Bit            = 0;
                uint8_t switch8Bit              = 0;
                uint16_t switch_threshold_open  = 0;
                uint16_t switch_threshold_close = 0;

                switch_direction_t switch_slip_direction;
                switch_status_type_t switch_status;
                chain_slip_mode_t switch_slip_report_mode;
                switch_report_type_t swwitch_trig_type;
                chain_status = M5Chain.getSwitch12BitAdc(devices_list->devices[i].id, &switch12Bit);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("Switch ID[%d] switch12Bit:%d \r\n", devices_list->devices[i].id, switch12Bit);
                } else {
                    Serial.printf("Switch ID[%d] get 12bit adc failed, chain_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status);
                }
                chain_status = M5Chain.getSwitch8BitAdc(devices_list->devices[i].id, &switch8Bit);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("Switch ID[%d] switch8Bit:%d \r\n", devices_list->devices[i].id, switch8Bit);
                } else {
                    Serial.printf("Switch ID[%d] get 8bit adc failed, chain_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status);
                }
                chain_status = M5Chain.getSwitchDirection(devices_list->devices[i].id, &switch_slip_direction);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("Switch ID[%d] switch_slip_direction:%d \r\n", devices_list->devices[i].id,
                                  switch_slip_direction);
                } else {
                    Serial.printf("Switch ID[%d] get slip direction failed, chain_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status);
                }
                chain_status = M5Chain.getSwitchStatus(devices_list->devices[i].id, &switch_status);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("Switch ID[%d] switch status: %d \r\n", devices_list->devices[i].id, switch_status);
                } else {
                    Serial.printf("Switch ID[%d] get slip direction failed, chain_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status);
                }
                chain_status = M5Chain.getSwitchAutoTriggerMode(devices_list->devices[i].id, &switch_slip_report_mode);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("Switch ID[%d] switch_slip report mode:%d \r\n", devices_list->devices[i].id,
                                  switch_slip_report_mode);
                } else {
                    Serial.printf("Switch ID[%d] get slip direction failed, chain_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status);
                }

                while (M5Chain.getSwitchTrigger(devices_list->devices[i].id, &swwitch_trig_type)) {
                    switch (swwitch_trig_type) {
                        case CHAIN_SWITCH_TRIGGER_REPORT_CLOSE:
                            Serial.printf("Switch ID[%d] slip status is: close \r\n", devices_list->devices[i].id);
                            break;
                        case CHAIN_SWITCH_TRIGGER_REPORT_OPEN:
                            Serial.printf("Switch ID[%d] slip status is: open \r\n", devices_list->devices[i].id);
                            break;
                        default:
                            Serial.printf("Switch ID[%d] slip status is: unknown, %04x \r\n",
                                          devices_list->devices[i].id, swwitch_trig_type);
                            break;
                    }
                }
            }
        }
    }
    delay(500);
}

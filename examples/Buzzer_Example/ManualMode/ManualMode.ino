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

void readBuzzerData(uint8_t id)
{
    buzzer_mode_t buzzer_mode;
    uint16_t freq;
    uint8_t duty;
    buzzer_status_t buzzerStatus;

    chain_status = M5Chain.getBuzzerMode(id, &buzzer_mode);
    if (chain_status == CHAIN_OK) {
        Serial.printf("Buzzer ID[%d] get mode success, mode:%d \r\n", id, buzzer_mode);
    } else {
        Serial.printf("Buzzer ID[%d] get mode failed, chain_status:%d \r\n", id, chain_status);
    }

    chain_status = M5Chain.getBuzzerFreq(id, &freq);
    if (chain_status == CHAIN_OK) {
        Serial.printf("Buzzer ID[%d] get freq success, freq:%d \r\n", id, freq);
    } else {
        Serial.printf("Buzzer ID[%d] get freq failed, chain_status:%d \r\n", id, chain_status);
    }

    chain_status = M5Chain.getBuzzerDuty(id, &duty);
    if (chain_status == CHAIN_OK) {
        Serial.printf("Buzzer ID[%d] get duty success, duty:%d \r\n", id, duty);
    } else {
        Serial.printf("Buzzer ID[%d] get duty failed, chain_status:%d \r\n", id, chain_status);
    }

    chain_status = M5Chain.getBuzzerStatus(id, &buzzerStatus);
    if (chain_status == CHAIN_OK) {
        Serial.printf("Buzzer ID[%d] get status success, buzzerStatus:%d \r\n", id, buzzerStatus);
    } else {
        Serial.printf("Buzzer ID[%d] get status failed, chain_status:%d \r\n", id, chain_status);
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
                    M5Chain.setBuzzerMode(devices_list->devices[i].id, BUZZER_MODE_MANUAL_PLAY, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("Buzzer ID[%d] set mode success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("Buzzer ID[%d] set mode failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                chain_status = M5Chain.setBuzzerFreq(devices_list->devices[i].id, 4000, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("Buzzer ID[%d] set freq success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("Buzzer ID[%d] set freq failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                chain_status = M5Chain.setBuzzerDuty(devices_list->devices[i].id, 50, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("Buzzer ID[%d] set duty success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("Buzzer ID[%d] set duty failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                chain_status =
                    M5Chain.setBuzzerStatus(devices_list->devices[i].id, BUZZER_STATUS_OFF, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("Buzzer ID[%d] set status success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("Buzzer ID[%d] set status failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                readBuzzerData(devices_list->devices[i].id);
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

    if (devices_list) {
        for (uint8_t i = 0; i < devices_list->count; i++) {
            if (devices_list->devices[i].device_type == CHAIN_BUZZER_TYPE_CODE) {
                Serial.printf("Buzzer ID[%d] Set random Freq: %d Hz, Duty: %d %%\r\n", devices_list->devices[i].id,
                              randFreq, randDuty);

                chain_status = M5Chain.setBuzzerFreq(devices_list->devices[i].id, randFreq, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("Buzzer ID[%d] set freq success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("Buzzer ID[%d] set freq failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                chain_status = M5Chain.setBuzzerDuty(devices_list->devices[i].id, randDuty, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("Buzzer ID[%d] set duty success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("Buzzer ID[%d] set duty failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                for (int j = 0; j < 3; j++) {
                    chain_status =
                        M5Chain.setBuzzerStatus(devices_list->devices[i].id, BUZZER_STATUS_ON, &operation_status);
                    if (chain_status == CHAIN_OK && operation_status) {
                        Serial.printf("Buzzer ID[%d] set status success\r\n", devices_list->devices[i].id);
                    } else {
                        Serial.printf("Buzzer ID[%d] set status failed, chain_status:%d  operation_status:%d \r\n",
                                      devices_list->devices[i].id, chain_status, operation_status);
                    }
                    delay(200);

                    chain_status =
                        M5Chain.setBuzzerStatus(devices_list->devices[i].id, BUZZER_STATUS_OFF, &operation_status);
                    if (chain_status == CHAIN_OK && operation_status) {
                        Serial.printf("Buzzer ID[%d] set status success\r\n", devices_list->devices[i].id);
                    } else {
                        Serial.printf("Buzzer ID[%d] set status failed, chain_status:%d  operation_status:%d \r\n",
                                      devices_list->devices[i].id, chain_status, operation_status);
                    }
                    delay(200);
                }
            }
        }
    }

    delay(1000);
}

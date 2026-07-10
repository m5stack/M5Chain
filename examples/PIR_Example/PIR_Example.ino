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

uint8_t uid32[4]  = {0};
uint8_t uid96[12] = {0};

uint8_t keep_seconds = 5;

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
    Serial.println("M5Chain PIR Test");
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
            if (devices_list->devices[i].device_type == CHAIN_PIR_TYPE_CODE) {
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

                uint8_t fw_version = 0;
                chain_status       = M5Chain.getFirmwareVersion(devices_list->devices[i].id, &fw_version, 100);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] get firmware version: 0x%02x\r\n", devices_list->devices[i].id, fw_version);
                } else {
                    Serial.printf("ID[%d] get firmware version failed, chain_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status);
                }

                chain_status = M5Chain.setPIRDetectTriggerMode(devices_list->devices[i].id, CHAIN_DETECT_REPORT_MODE,
                                                               &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("ID[%d] set pir detect trigger mode success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf(
                        "ID[%d] set pir detect trigger mode failed, chain_status:%d  operation_status:%d \r\n",
                        devices_list->devices[i].id, chain_status, operation_status);
                }

                uint8_t saveToFlash = CHAIN_SAVE_FLASH_DISABLE;
                chain_status        = M5Chain.setPIRComeTriggerKeepSeconds(devices_list->devices[i].id, keep_seconds,
                                                                           &operation_status, saveToFlash);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf(
                        "ID[%d] set pir person-come trigger keep seconds success, keep %us, operate_status: %u\r\n",
                        devices_list->devices[i].id, keep_seconds, operation_status);
                } else {
                    Serial.printf(
                        "ID[%d] set pir person-come trigger keep seconds failed, chain_status:%d  "
                        "operation_status:%d\r\n",
                        devices_list->devices[i].id, chain_status, operation_status);
                }

                chain_status = M5Chain.getPIRComeTriggerKeepSeconds(devices_list->devices[i].id, &keep_seconds);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("ID[%d] get pir person-come trigger keep seconds success, keep %us\r\n",
                                  devices_list->devices[i].id, keep_seconds);
                } else {
                    Serial.printf("ID[%d] get pir person-come trigger keep seconds failed, chain_status:%d\r\n",
                                  devices_list->devices[i].id, chain_status);
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
            if (devices_list->devices[i].device_type == CHAIN_PIR_TYPE_CODE) {
                chain_detect_mode_t pir_detect_report_mode;
                pir_detect_result_t pir_detect_res;
                pir_detect_report_t pir_detect_report_res;
                chain_status = M5Chain.getPIRDetectTriggerMode(devices_list->devices[i].id, &pir_detect_report_mode);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("Switch ID[%d] pir detect_trigger mode:%d \r\n", devices_list->devices[i].id,
                                  pir_detect_report_mode);
                } else {
                    Serial.printf("Switch ID[%d] get pir_detect_trigger_mode failed, chain_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status);
                }
                chain_status = M5Chain.getIRStatus(devices_list->devices[i].id, &pir_detect_res);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("PIR ID[%d] detect result is: %d\r\n", devices_list->devices[i].id, pir_detect_res);
                } else {
                    Serial.printf("PIR ID[%d] get ir status failed, chain_status:%d \r\n", devices_list->devices[i].id,
                                  chain_status);
                }
                while (M5Chain.getPIRDetectTrigger(devices_list->devices[i].id, &pir_detect_report_res)) {
                    switch (pir_detect_report_res) {
                        case CHAIN_PIR_REPORT_PERSON_LEAVE:
                            Serial.printf("ENCODER ID[%d] detect result is: non person \r\n",
                                          devices_list->devices[i].id);
                            break;
                        case CHAIN_PIR_REPORT_PERSON_COME:
                            Serial.printf("ENCODER ID[%d] detect result is: have person \r\n",
                                          devices_list->devices[i].id);
                            break;
                        default:
                            Serial.printf("ENCODER ID[%d] detect result is: unknown , pir_detect_res:%04x\r\n",
                                          devices_list->devices[i].id, pir_detect_res);
                            break;
                    }
                }
            }
        }
    }
    delay(500);
}

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
chain_status_t chain_status;
uint16_t device_nums     = 0;
uint8_t operation_status = 0;

uint8_t rgb_test[5][3] = {
    {0xFF, 0x00, 0x00}, {0x00, 0xFF, 0x00}, {0x00, 0x00, 0xFF}, {0xFF, 0xFF, 0xFF}, {0x00, 0x00, 0x00},
};

void printDeviceList(device_list_t *devices);
void printWorkStatus(work_status_t gpio11_status, work_status_t gpio12_status);
void setup()
{
    Serial.begin(115200);
    Serial.println("M5Chain ToF Test");
    M5Chain.begin(&Serial2, 115200, RXD_PIN, TXD_PIN);

    if (M5Chain.isDeviceConnected()) {
        Serial.println("devices is connected");
        chain_status = M5Chain.getDeviceNum(&device_nums);
        if (chain_status == CHAIN_OK) {
            devices_list          = (device_list_t *)malloc(sizeof(device_list_t));
            devices_list->count   = device_nums;
            devices_list->devices = (device_info_t *)malloc(sizeof(device_info_t) * device_nums);
            if (M5Chain.getDeviceList(devices_list)) {
                Serial.println("devices list get success");
                printDeviceList(devices_list);
            } else {
                Serial.println("devices list get failed");
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
            if (devices_list->devices[i].device_type == UNIT_CHAIN_BUS_TYPE_CODE) {
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
                chain_status = M5Chain.setChainBusI2cMode(devices_list->devices[i].id, CHAIN_I2C_HIGH_SPEED_400KHZ,
                                                          &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("UART ID[%d] set i2c mode success \r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("UART ID[%d] set i2c mode fail, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }
            }
        }
    } else {
        Serial.println("devices list is NULL");
    }

    delay(5);
}

uint8_t i2c_scan_addr[128] = {0};
uint8_t i2c_scan_addr_num  = 0;
work_status_t gpio1;
work_status_t gpio2;
void loop()
{
    if (devices_list) {
        for (uint8_t i = 0; i < devices_list->count; i++) {
            if (devices_list->devices[i].device_type == UNIT_CHAIN_BUS_TYPE_CODE) {
                chain_status = M5Chain.getChainBusWorkMode(devices_list->devices[i].id, &gpio1, &gpio2);
                if (chain_status == CHAIN_OK) {
                    printWorkStatus((work_status_t)gpio1, (work_status_t)gpio2);
                    if (gpio1 == CHAIN_I2C_WORK_STATUS && gpio2 == CHAIN_I2C_WORK_STATUS) {
                        chain_status =
                            M5Chain.getChainBusI2cScanAddr(devices_list->devices[i].id, &i2c_scan_addr_num,
                                                           i2c_scan_addr, sizeof(i2c_scan_addr), &operation_status);
                        if (chain_status == CHAIN_OK && operation_status == 1) {
                            if (operation_status == 1) {
                                Serial.printf("UART ID[%d] scan i2c device success, num: %d \r\n",
                                              devices_list->devices[i].id, i2c_scan_addr_num);
                                for (uint8_t j = 0; j < i2c_scan_addr_num; j++) {
                                    Serial.printf("UART ID[%d] i2c addr%d:0x%02x \r\n", devices_list->devices[i].id, j,
                                                  i2c_scan_addr[j]);
                                }
                            }
                        } else {
                            Serial.printf(
                                "UART ID[%d] get i2c scan addr fail, chain_status:%d  operation_status:%d \r\n",
                                devices_list->devices[i].id, chain_status, operation_status);
                        }
                    }
                } else {
                    Serial.printf("UART ID[%d] get work mode fail, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }
            }
        }

        if (M5Chain.getEnumPleaseNum()) {
            Serial.println("enum please");
        }
        delay(1000);
    }
}
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

void printWorkStatus(work_status_t gpio11_status, work_status_t gpio12_status)
{
    Serial.print("  >>> GPIO_1: ");
    switch (gpio11_status) {
        case CHAIN_NOT_WORK_STATUS:
            Serial.println("Not configured working status");
            break;
        case CHAIN_OUTPUT_WORK_STATUS:
            Serial.println("Output status");
            break;
        case CHAIN_INPUT_WORK_STATUS:
            Serial.println("Input status");
            break;
        case CHAIN_NVIC_WORK_STATUS:
            Serial.println("External interrupt working status");
            break;
        case CHAIN_ADC_WORK_STATUS:
            Serial.println("ADC working status");
            break;
        case CHAIN_I2C_WORK_STATUS:
            Serial.println("I2C working status");
            break;
        default:
            Serial.println("Unrecognized work status");
            break;
    }

    Serial.print("  >>> GPIO_2: ");
    switch (gpio12_status) {
        case CHAIN_NOT_WORK_STATUS:
            Serial.println("Not configured working status.");
            break;
        case CHAIN_OUTPUT_WORK_STATUS:
            Serial.println("Output status.");
            break;
        case CHAIN_INPUT_WORK_STATUS:
            Serial.println("Input status.");
            break;
        case CHAIN_NVIC_WORK_STATUS:
            Serial.println("External interrupt working status.");
            break;
        case CHAIN_ADC_WORK_STATUS:
            Serial.println("ADC working status.");
            break;
        case CHAIN_I2C_WORK_STATUS:
            Serial.println("I2C working status.");
            break;
        default:
            Serial.println("Unrecognized work status.");
            break;
    }
}

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

chain_dlight_event_trigger_t dlight_lux_trigger_res;

static void printSetResult(uint8_t id, const char *name, chain_status_t status, uint8_t operationStatus)
{
    if (status == CHAIN_OK && operationStatus) {
        Serial.printf("ID[%d] %s success, operation_status:%d\r\n", id, name, operationStatus);
    } else {
        Serial.printf("ID[%d] %s failed, chain_status:%d operation_status:%d\r\n", id, name, status, operationStatus);
    }
}

static void testDLightInterfaces(uint8_t id)
{
    uint32_t lux                         = 0;
    uint32_t high_threshold              = 0;
    uint32_t low_threshold               = 0;
    chain_dlight_enable_t enable         = CHAIN_DLIGHT_DISABLE;
    chain_dlight_enable_t mode           = CHAIN_DLIGHT_DISABLE;
    chain_dlight_gain_t gain             = CHAIN_DLIGHT_GAIN_1X;
    chain_dlight_resolution_t resolution = CHAIN_DLIGHT_RESOLUTION_18BIT_100MS;
    chain_dlight_rate_t rate             = CHAIN_DLIGHT_RATE_100MS;

    Serial.printf("ID[%d] Chain DLight type code: 0x%04x\r\n", id, M5Chain.getDLightTypeCode());

    chain_status = M5Chain.getDLightLux(id, &lux);
    if (chain_status == CHAIN_OK) {
        Serial.printf("ID[%d] get dlight lux success, lux:%lu\r\n", id, (unsigned long)lux);
    } else {
        Serial.printf("ID[%d] get dlight lux failed, chain_status:%d\r\n", id, chain_status);
    }

    operation_status = 0;
    chain_status     = M5Chain.setDLightLuxInterruptEnable(id, CHAIN_DLIGHT_DISABLE, &operation_status);
    printSetResult(id, "set dlight lux interrupt disable", chain_status, operation_status);

    chain_status = M5Chain.getDLightLuxInterruptEnable(id, &enable);
    if (chain_status == CHAIN_OK) {
        Serial.printf("ID[%d] get dlight lux interrupt enable success, enable:%d\r\n", id, enable);
    } else {
        Serial.printf("ID[%d] get dlight lux interrupt enable failed, chain_status:%d\r\n", id, chain_status);
    }

    operation_status = 0;
    chain_status     = M5Chain.setDLightLuxInterruptThreshold(id, 2047, 120, &operation_status);
    printSetResult(id, "set dlight lux interrupt threshold", chain_status, operation_status);

    chain_status = M5Chain.getDLightLuxInterruptThreshold(id, &high_threshold, &low_threshold);
    if (chain_status == CHAIN_OK) {
        Serial.printf("ID[%d] get dlight lux interrupt threshold success, high:%lu low:%lu\r\n", id,
                      (unsigned long)high_threshold, (unsigned long)low_threshold);
    } else {
        Serial.printf("ID[%d] get dlight lux interrupt threshold failed, chain_status:%d\r\n", id, chain_status);
    }

    operation_status = 0;
    chain_status     = M5Chain.setDLightSensorGain(id, CHAIN_DLIGHT_GAIN_6X, &operation_status);
    printSetResult(id, "set dlight sensor gain", chain_status, operation_status);

    chain_status = M5Chain.getDLightSensorGain(id, &gain);
    if (chain_status == CHAIN_OK) {
        Serial.printf("ID[%d] get dlight sensor gain success, gain:%d\r\n", id, gain);
    } else {
        Serial.printf("ID[%d] get dlight sensor gain failed, chain_status:%d\r\n", id, chain_status);
    }

    operation_status = 0;
    chain_status     = M5Chain.setDLightSensorResolution(id, CHAIN_DLIGHT_RESOLUTION_18BIT_100MS, &operation_status);
    printSetResult(id, "set dlight sensor resolution", chain_status, operation_status);

    chain_status = M5Chain.getDLightSensorResolution(id, &resolution);
    if (chain_status == CHAIN_OK) {
        Serial.printf("ID[%d] get dlight sensor resolution success, resolution:%d\r\n", id, resolution);
    } else {
        Serial.printf("ID[%d] get dlight sensor resolution failed, chain_status:%d\r\n", id, chain_status);
    }

    operation_status = 0;
    chain_status     = M5Chain.setDLightSensorRate(id, CHAIN_DLIGHT_RATE_500MS, &operation_status);
    printSetResult(id, "set dlight sensor rate", chain_status, operation_status);

    chain_status = M5Chain.getDLightSensorRate(id, &rate);
    if (chain_status == CHAIN_OK) {
        Serial.printf("ID[%d] get dlight sensor rate success, rate:%d\r\n", id, rate);
    } else {
        Serial.printf("ID[%d] get dlight sensor rate failed, chain_status:%d\r\n", id, chain_status);
    }

    operation_status = 0;
    chain_status     = M5Chain.setDLightLuxInterruptEventTriggerMode(id, CHAIN_DLIGHT_DISABLE, &operation_status);
    printSetResult(id, "disable dlight lux interrupt event trigger mode", chain_status, operation_status);

    operation_status = 0;
    chain_status     = M5Chain.setDLightLuxInterruptEnable(id, CHAIN_DLIGHT_ENABLE, &operation_status);
    printSetResult(id, "set dlight lux interrupt enable", chain_status, operation_status);

    operation_status = 0;
    chain_status     = M5Chain.setDLightLuxInterruptEventTriggerMode(id, CHAIN_DLIGHT_ENABLE, &operation_status);
    printSetResult(id, "set dlight lux interrupt event trigger mode", chain_status, operation_status);

    chain_status = M5Chain.getDLightLuxInterruptEventTriggerMode(id, &mode);
    if (chain_status == CHAIN_OK) {
        Serial.printf("ID[%d] get dlight lux interrupt event trigger mode success, mode:%d\r\n", id, mode);
    } else {
        Serial.printf("ID[%d] get dlight lux interrupt event trigger mode failed, chain_status:%d\r\n", id,
                      chain_status);
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

void setup()
{
    Serial.begin(115200);
    Serial.println("M5Chain Dlight Test");
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
            if (devices_list->devices[i].device_type == CHAIN_DLIGHT_TYPE_CODE) {
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

                testDLightInterfaces(devices_list->devices[i].id);
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
            if (devices_list->devices[i].device_type == CHAIN_DLIGHT_TYPE_CODE) {
                uint32_t lux = 0;
                chain_status = M5Chain.getDLightLux(devices_list->devices[i].id, &lux);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("DLight ID[%d] lux:%lu\r\n", devices_list->devices[i].id, (unsigned long)lux);
                } else {
                    Serial.printf("DLight ID[%d] get lux failed, chain_status:%d\r\n", devices_list->devices[i].id,
                                  chain_status);
                }
                while (M5Chain.getDLightLuxTrigger(devices_list->devices[i].id, &dlight_lux_trigger_res)) {
                    uint16_t event_record = (uint16_t)dlight_lux_trigger_res;
                    if ((event_record >> 8) != (uint16_t)CHAIN_DLIGHT_TYPE_CODE) {
                        Serial.printf("DLight ID[%d] unknown event type: 0x%04X\r\n", devices_list->devices[i].id,
                                      event_record);
                        continue;
                    }
                    uint8_t event_status = (uint8_t)(event_record & 0xFFU);
                    switch (event_status) {
                        case CHAIN_DLIGHT_REPORT_LUX_LOW:
                            Serial.printf("DLight ID[%d] low threshold triggered\r\n", devices_list->devices[i].id);
                            break;
                        case CHAIN_DLIGHT_REPORT_LUX_HIGH:
                            Serial.printf("DLight ID[%d] high threshold triggered\r\n", devices_list->devices[i].id);
                            break;
                        case CHAIN_DLIGHT_RELEASE_LUX_LOW:
                            Serial.printf("DLight ID[%d] low threshold released\r\n", devices_list->devices[i].id);
                            break;
                        case CHAIN_DLIGHT_RELEASE_LUX_HIGH:
                            Serial.printf("DLight ID[%d] high threshold released\r\n", devices_list->devices[i].id);
                            break;
                        default:
                            Serial.printf("DLight ID[%d] unknown event status: 0x%02X\r\n", devices_list->devices[i].id,
                                          event_status);
                            break;
                    }
                }
            }
        }
    }
    delay(500);
}

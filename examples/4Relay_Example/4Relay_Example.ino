/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Chain 4Relay example
 *
 * This example demonstrates the common RGB interface together with the
 * 4Relay command interface:
 * - set and read RGB colors;
 * - set one relay channel and read its actual level;
 * - set all four relay channels with one request and read them back.
 */

#include "M5Chain.h"

#define TXD_PIN GPIO_NUM_21  // Tx
#define RXD_PIN GPIO_NUM_22  // Rx

Chain M5Chain;

device_list_t *devices_list = nullptr;
uint16_t device_nums        = 0;
uint8_t operation_status    = 0;
chain_status_t chain_status = CHAIN_OK;

uint8_t rgb_test[5][3] = {
    {0xFF, 0x00, 0x00}, {0x00, 0xFF, 0x00}, {0x00, 0x00, 0xFF}, {0xFF, 0xFF, 0xFF}, {0x00, 0x00, 0x00},
};

static void printDeviceList(device_list_t *devices)
{
    if (devices == nullptr) {
        Serial.println("devices is NULL");
        return;
    }

    Serial.print("devices count: ");
    Serial.println(devices->count);

    for (uint16_t i = 0; i < devices->count; i++) {
        Serial.print("devices ID: ");
        Serial.println(devices->devices[i].id);
        Serial.print("devices type: 0x");
        Serial.println(devices->devices[i].device_type, HEX);
    }
}

static void printSetResult(uint16_t id, const char *name, chain_status_t status, uint8_t operationStatus)
{
    if (status == CHAIN_OK && operationStatus == 1) {
        Serial.printf("ID[%u] %s success\r\n", id, name);
    } else {
        Serial.printf("ID[%u] %s failed, chain_status:%d operation_status:%u\r\n", id, name, status, operationStatus);
    }
}

static void printRelayBits(uint8_t levels)
{
    levels &= 0x0F;
    Serial.printf("0b%u%u%u%u", (levels >> 3) & 0x01, (levels >> 2) & 0x01, (levels >> 1) & 0x01, levels & 0x01);
}

static void testRGB(uint16_t id)
{
    operation_status = 0;
    chain_status     = M5Chain.setRGBLight(id, 100, &operation_status);
    printSetResult(id, "set RGB brightness", chain_status, operation_status);

    for (uint8_t i = 0; i < 5; i++) {
        operation_status = 0;
        chain_status     = M5Chain.setRGBValue(id, 0, 1, rgb_test[i], 3, &operation_status);
        if (chain_status == CHAIN_OK && operation_status == 1) {
            Serial.printf("ID[%u] set RGB %u %u %u success\r\n", id, rgb_test[i][0], rgb_test[i][1], rgb_test[i][2]);
        } else {
            Serial.printf("ID[%u] set RGB %u %u %u failed, chain_status:%d operation_status:%u\r\n", id, rgb_test[i][0],
                          rgb_test[i][1], rgb_test[i][2], chain_status, operation_status);
        }

        uint8_t rgb[3]   = {0};
        operation_status = 0;
        chain_status     = M5Chain.getRGBValue(id, 0, 1, rgb, 3, &operation_status);
        if (chain_status == CHAIN_OK && operation_status == 1) {
            Serial.printf("ID[%u] get RGB %u %u %u success\r\n", id, rgb[0], rgb[1], rgb[2]);
        } else {
            Serial.printf("ID[%u] get RGB failed, chain_status:%d operation_status:%u\r\n", id, chain_status,
                          operation_status);
        }
        delay(500);
    }
}

static bool readRelayLevels(uint8_t id, uint8_t *levels)
{
    chain_status = M5Chain.get4RelayLevels(id, levels);
    if (chain_status != CHAIN_OK) {
        Serial.printf("4Relay ID[%u] read levels failed, chain_status:%d\r\n", id, chain_status);
        return false;
    }

    Serial.printf("4Relay ID[%u] actual levels: ", id);
    printRelayBits(*levels);
    Serial.println();
    return true;
}

static void testRelays(uint8_t id)
{
    // Demonstrate single-channel writes and verify the selected channel.
    for (uint8_t index = 0; index < CHAIN_4RELAY_CHANNEL_COUNT; index++) {
        uint8_t level    = index & 0x01;
        operation_status = 0;
        chain_status     = M5Chain.set4RelayLevel(id, index, level, &operation_status);
        char message[32] = {0};
        snprintf(message, sizeof(message), "set relay %u to %u", index, level);
        printSetResult(id, message, chain_status, operation_status);

        uint8_t actual = 0;
        if (readRelayLevels(id, &actual)) {
            bool matched = (((actual >> index) & 0x01) == level);
            Serial.printf("4Relay ID[%u] channel %u expected:%u actual:%u %s\r\n", id, index, level,
                          (actual >> index) & 0x01, matched ? "match" : "mismatch");
        }
        delay(300);
    }

    // Demonstrate the one-byte all-channel request and verify all four bits.
    const uint8_t levels = 0b0101;
    operation_status     = 0;
    chain_status         = M5Chain.set4RelayLevels(id, levels, &operation_status);
    printSetResult(id, "set all relays", chain_status, operation_status);

    uint8_t actual = 0;
    if (readRelayLevels(id, &actual)) {
        Serial.printf("4Relay ID[%u] expected levels: ", id);
        printRelayBits(levels);
        Serial.printf(" %s\r\n", ((actual & 0x0F) == levels) ? "match" : "mismatch");
    }

    // Leave the relay outputs in a safe all-low state after the demonstration.
    operation_status = 0;
    chain_status     = M5Chain.set4RelayLevels(id, 0b0000, &operation_status);
    printSetResult(id, "clear all relays", chain_status, operation_status);
}

void setup()
{
    Serial.begin(115200);
    Serial.println("M5Chain 4Relay Test");
    M5Chain.begin(&Serial2, 115200, RXD_PIN, TXD_PIN);

    if (!M5Chain.isDeviceConnected()) {
        Serial.println("devices is not connected.");
        return;
    }

    Serial.println("devices is connected");
    chain_status = M5Chain.getDeviceNum(&device_nums);
    if (chain_status != CHAIN_OK) {
        Serial.printf("devices num get failed, chain_status:%d\r\n", chain_status);
        return;
    }

    devices_list = (device_list_t *)malloc(sizeof(device_list_t));
    if (devices_list == nullptr) {
        Serial.println("allocate devices list failed");
        return;
    }

    devices_list->count   = device_nums;
    devices_list->devices = (device_info_t *)malloc(sizeof(device_info_t) * device_nums);
    if (devices_list->devices == nullptr || !M5Chain.getDeviceList(devices_list)) {
        Serial.println("get devices list failed");
        return;
    }

    Serial.println("get devices list success");
    printDeviceList(devices_list);

    for (uint16_t i = 0; i < devices_list->count; i++) {
        if (devices_list->devices[i].device_type != CHAIN_4RELAY_TYPE_CODE) {
            continue;
        }

        uint16_t id = devices_list->devices[i].id;
        testRGB(id);
        testRelays((uint8_t)id);
        Serial.printf("4Relay ID[%u] type code: 0x%04X\r\n", id, M5Chain.get4RelayTypeCode());
    }
}

void loop()
{
    if (devices_list == nullptr || devices_list->devices == nullptr) {
        delay(1000);
        return;
    }

    for (uint16_t i = 0; i < devices_list->count; i++) {
        if (devices_list->devices[i].device_type != CHAIN_4RELAY_TYPE_CODE) {
            continue;
        }

        uint8_t levels = 0;
        readRelayLevels((uint8_t)devices_list->devices[i].id, &levels);
    }
    delay(1000);
}

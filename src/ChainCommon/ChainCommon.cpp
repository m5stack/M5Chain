/*
 *SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#include "ChainCommon.hpp"

bool ChainCommon::acquireMutex(void)
{
    unsigned long start = millis();
    while (true) {
        if (!mutexLocked) {
            mutexLocked = true;
            return true;
        }
        if (millis() - start >= 1000) {
            return false;
        }
        delay(1);
    }
}

void ChainCommon::releaseMutex(void)
{
    mutexLocked = false;
}

void ChainCommon::begin(HardwareSerial *serial, unsigned long baud, int8_t rxPin, int8_t txPin, uint32_t config,
                        bool invert, unsigned long timeout_ms, uint8_t rxfifo_full_thrhd)
{
    serialPort = serial;
    serialPort->begin(baud, SERIAL_8N1, rxPin, txPin, invert, timeout_ms, rxfifo_full_thrhd);
}

uint8_t ChainCommon::calculateCRC(const uint8_t *buffer, uint16_t size)
{
    uint8_t crc8 = 0;
    for (uint8_t i = 4; i < (size - 3); i++) {
        crc8 += buffer[i];
    }
    return crc8;
}

bool ChainCommon::checkPacket(const uint8_t *buffer, uint16_t size)
{
    if (size < PACK_SIZE_MIN) {
        return false;
    }

    if (buffer[0] != PACK_HEAD_HIGH || buffer[1] != PACK_HEAD_LOW || buffer[size - 1] != PACK_END_LOW ||
        buffer[size - 2] != PACK_END_HIGH) {
        return false;
    }

    uint16_t length = ((uint16_t)buffer[3] << 8) | buffer[2];
    length += 6;
    if (length != size) {
        return false;
    }

    uint8_t crc8 = calculateCRC(buffer, size);
    if (crc8 != buffer[size - 3]) {
        return false;
    }

    return true;
}

void ChainCommon::sendPacket(uint16_t id, uint8_t cmd, const uint8_t *buffer, uint16_t size)
{
    uint16_t cmdSize = 3 + size;  // id + cmd + data + crc8
    sendBufferSize   = size + 9;

    sendBuffer[0] = PACK_HEAD_HIGH;
    sendBuffer[1] = PACK_HEAD_LOW;
    sendBuffer[2] = cmdSize & 0xFF;
    sendBuffer[3] = (cmdSize >> 8) & 0xFF;
    sendBuffer[4] = id;
    sendBuffer[5] = cmd;

    memcpy(sendBuffer + 6, buffer, size);

    uint8_t crc8                   = calculateCRC(sendBuffer, sendBufferSize);
    sendBuffer[sendBufferSize - 3] = crc8;
    sendBuffer[sendBufferSize - 2] = PACK_END_HIGH;
    sendBuffer[sendBufferSize - 1] = PACK_END_LOW;

#ifdef CHAIN_DEBUG
    Serial.printf("ID: 0x%02X, CMD: 0x%02X\n", id, cmd);
    Serial.printf("TX Packet (%d bytes):\n", sendBufferSize);
    for (int i = 0; i < sendBufferSize; i++) {
        Serial.printf("0x%02X ", sendBuffer[i]);
        if ((i + 1) % 16 == 0) Serial.println();
    }
    Serial.println();
#endif

    if (serialPort) {
        serialPort->write(sendBuffer, sendBufferSize);
    } else {
#ifdef CHAIN_DEBUG
        Serial.println("Serial port not initialized.");
#endif
    }
}

bool ChainCommon::addRecord(record_list_t *list, record_info_t info)
{
    record_node_t *newNode = (record_node_t *)malloc(sizeof(record_node_t));
    if (!newNode) {
        return false;
    }
    newNode->info = info;
    newNode->next = NULL;

    if (list->head == NULL) {
        list->head = newNode;
    } else {
        record_node_t *current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
    list->count++;
    return true;
}

bool ChainCommon::findRecord(record_list_t *list, uint8_t id, record_info_t *result)
{
    record_node_t *current  = list->head;
    record_node_t *previous = NULL;

    while (current) {
        if (current->info.id == id) {
            if (result) {
                *result = current->info;
            }

            if (previous) {
                previous->next = current->next;
            } else {
                list->head = current->next;
            }

            free(current);
            list->count--;
            return true;
        }
        previous = current;
        current  = current->next;
    }
    return false;
}

bool ChainCommon::processBufferData(uint16_t id, uint8_t cmd)
{
    uint16_t startIndex = 0;
    bool packetFound    = false;

    // Continue scanning while at least minimum packet size bytes remain
    while ((receiveBufferSize - startIndex) >= PACK_SIZE_MIN) {
        // Check for packet header 0xAA 0x55
        if (receiveBuffer[startIndex] == 0xAA && receiveBuffer[startIndex + 1] == 0x55) {
            // Read 2-byte packet length (little endian)
            uint16_t length = (uint16_t)receiveBuffer[startIndex + 2] | ((uint16_t)receiveBuffer[startIndex + 3] << 8);

            // Calculate total packet size: header(2) + length(2) + payload(length) + footer(2)
            uint16_t packetSize = 4 + length + 2;

            // Check if full packet data is available in buffer
            if ((receiveBufferSize - startIndex) < packetSize) {
                // Incomplete packet, wait for more data
                break;
            }

            // Verify packet footer bytes
            if (receiveBuffer[startIndex + packetSize - 2] != 0x55 ||
                receiveBuffer[startIndex + packetSize - 1] != 0xAA) {
                // Invalid footer, skip this byte and continue searching
                startIndex++;
                continue;
            }

            uint8_t *packetData = receiveBuffer + startIndex;
            uint8_t packetId    = packetData[4];
            uint8_t packetCmd   = packetData[5];

            // Verify packet CRC checksum
            if (!checkPacket(packetData, packetSize)) {
                // CRC error, skip byte and continue searching
                startIndex++;
                continue;
            }

            // Check for matching device ID and command
            if (packetId == id && packetCmd == cmd) {
                memcpy(returnPacket, packetData, packetSize);
                returnPacketSize = packetSize;
                packetFound      = true;
            }
            // Handle special enumeration command
            else if (packetCmd == CHAIN_ENUM_PLEASE) {
                enumPleaseCount++;
            }
            // Handle specific command 0xE0 for recording data
            else if (packetCmd == 0xE0) {
                if (recordList.count < ACTIVE_PACKET_RECORD_SIZE) {
                    uint16_t val = ((uint16_t)packetData[7] << 8) | packetData[6];
                    addRecord(&recordList, (record_info_t){packetId, val});
                }
            }

#ifdef CHAIN_DEBUG
            Serial.printf("RX Packet (%d bytes):\n", packetSize);
            for (int i = 0; i < packetSize; i++) {
                Serial.printf("0x%02X ", packetData[i]);
                if ((i + 1) % 16 == 0) Serial.println();
            }
            Serial.println();
#endif

            // Remove processed packet from buffer by shifting remaining data
            uint16_t remainingSize = receiveBufferSize - (startIndex + packetSize);
            if (remainingSize > 0) {
                memmove(receiveBuffer, receiveBuffer + startIndex + packetSize, remainingSize);
            }
            receiveBufferSize = remainingSize;

            // Reset start index to start scanning from buffer start again
            startIndex = 0;
        } else {
            // Not a packet header, move to next byte
            startIndex++;
        }
    }

    return packetFound;
}

void ChainCommon::processIncomingData(void)
{
    if (available() > 0) {
        readBuffer();
        processBufferData();
    }
}

bool ChainCommon::available(void)
{
    return serialPort->available() > 0;
}

void ChainCommon::readBuffer(void)
{
    unsigned long startTime = millis();
    while (millis() - startTime < TIMEOUT_MS) {
        if (available() && receiveBufferSize < RECEIVE_BUFFER_SIZE) {
            receiveBuffer[receiveBufferSize++] = serialPort->read();
            startTime                          = millis();
        }
    }
}

bool ChainCommon::waitForData(uint16_t id, uint8_t cmd, uint32_t timeout)
{
    uint32_t startTime = millis();
    while (millis() - startTime < timeout) {
        if (available()) {
            readBuffer();
            bool status = processBufferData(id, cmd);
            if (status) {
                return true;
            }
        }
        delay(1);
    }
    return false;
}

uint16_t ChainCommon::getEnumPleaseNum(void)
{
    processIncomingData();
    uint16_t temp   = enumPleaseCount;
    enumPleaseCount = 0;
    return temp;
}

chain_status_t ChainCommon::setRGBValue(uint16_t id, uint8_t index, uint8_t num, uint8_t *rgb, uint8_t size,
                                        uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (num * 3 != size) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = index;
        cmdBuffer[cmdBufferSize++] = num;
        for (uint8_t i = 0; i < num; i++) {
            cmdBuffer[cmdBufferSize++] = rgb[i * 3];
            cmdBuffer[cmdBufferSize++] = rgb[i * 3 + 1];
            cmdBuffer[cmdBufferSize++] = rgb[i * 3 + 2];
        }
        sendPacket(id, CHAIN_SET_RGB_VALUE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_SET_RGB_VALUE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainCommon::getRGBValue(uint16_t id, uint8_t index, uint8_t num, uint8_t *rgb, uint8_t size,
                                        uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (num * 3 != size) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = index;
        cmdBuffer[cmdBufferSize++] = num;
        sendPacket(id, CHAIN_GET_RGB_VALUE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_GET_RGB_VALUE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
                if (*operationStatus == 1) {
                    for (uint8_t i = 0; i < num; i++) {
                        rgb[i * 3 + index * 3]     = returnPacket[7 + i * 3];
                        rgb[i * 3 + 1 + index * 3] = returnPacket[8 + i * 3];
                        rgb[i * 3 + 2 + index * 3] = returnPacket[9 + i * 3];
                    }
                }

            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainCommon::setRGBLight(uint16_t id, uint8_t rgbBrightness, uint8_t *operationStatus,
                                        chain_save_flash_t saveToFlash, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (rgbBrightness > RGB_MAX_BRIGHTNESS) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = rgbBrightness;
        cmdBuffer[cmdBufferSize++] = saveToFlash;
        sendPacket(id, CHAIN_SET_RGB_LIGHT, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_SET_RGB_LIGHT, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainCommon::getRGBLight(uint16_t id, uint8_t *rgbBrightness, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_GET_RGB_LIGHT, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_GET_RGB_LIGHT, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *rgbBrightness = returnPacket[6];
            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainCommon::getUID(uint16_t id, uid_type_t uidType, uint8_t *uid, uint8_t size,
                                   uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    uint8_t expectedSize = (uidType == UID_TYPE_4_BYTE) ? 4 : 12;
    if (size < expectedSize) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = static_cast<uint8_t>(uidType);
        sendPacket(id, CHAIN_GET_UID, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_GET_UID, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
                if (*operationStatus == 1) {
                    for (uint8_t i = 0; i < expectedSize; i++) {
                        uid[i] = returnPacket[7 + i];
                    }
                }
            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainCommon::getBootloaderVersion(uint16_t id, uint8_t *bootloaderVersion, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_GET_BOOTLOADER_VERSION, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_GET_BOOTLOADER_VERSION, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *bootloaderVersion = returnPacket[6];
            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainCommon::getFirmwareVersion(uint16_t id, uint8_t *firmwareVersion, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_GET_VERSION_DEVICE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_GET_VERSION_DEVICE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *firmwareVersion = returnPacket[6];
            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainCommon::getDeviceType(uint16_t id, chain_device_type_t *deviceType, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_GET_DEVICE_TYPE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_GET_DEVICE_TYPE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *deviceType = (chain_device_type_t)((returnPacket[7] << 8) | returnPacket[6]);
            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainCommon::getDeviceNum(uint16_t *deviceNum, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = 0x00;
        sendPacket(0xFF, CHAIN_ENUM, cmdBuffer, cmdBufferSize);
        if (waitForData(0xFF, CHAIN_ENUM, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *deviceNum = returnPacket[6];
            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

bool ChainCommon::isDeviceConnected(uint8_t maxRetries, unsigned long timeout)
{
    bool status    = true;
    int retryCount = 0;
    if (acquireMutex()) {
        while (retryCount < maxRetries) {
            cmdBufferSize = 0;
            sendPacket(0xFF, CHAIN_HEARTBEAT, cmdBuffer, cmdBufferSize);
            if (waitForData(0xFF, CHAIN_HEARTBEAT, timeout)) {
                if (checkPacket(returnPacket, returnPacketSize)) {
                    break;
                }
            }
            retryCount++;
        }
        if (retryCount == maxRetries) {
            status = false;
        }
        releaseMutex();
    } else {
        status = false;
    }
    return status;
}

bool ChainCommon::getDeviceList(device_list_t *list, unsigned long timeout)
{
    chain_status_t status   = CHAIN_OK;
    unsigned long startTime = millis();
    for (uint16_t i = 0; i < list->count; i++) {
        list->devices[i].id = i + 1;
        status              = getDeviceType(list->devices[i].id, &list->devices[i].device_type, timeout);
        if (status != CHAIN_OK) {
            return false;
        }
    }
    return true;
}
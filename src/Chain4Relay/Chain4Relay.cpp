/*
 *SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#include "Chain4Relay/Chain4Relay.hpp"

chain_status_t Chain4Relay::send4RelayRequest(uint8_t id, uint8_t request, uint8_t *operationStatus,
                                              unsigned long timeout)
{
    if (id == 0 || operationStatus == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    chain_status_t status = CHAIN_OK;
    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = request;
        sendPacket(id, CHAIN_4RELAY_SET_LEVEL, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_4RELAY_SET_LEVEL, timeout)) {
            if (returnPacketSize == 10 && checkPacket(returnPacket, returnPacketSize) && returnPacket[6] <= 1) {
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

chain_status_t Chain4Relay::set4RelayLevel(uint8_t id, uint8_t index, uint8_t level, uint8_t *operationStatus,
                                           unsigned long timeout)
{
    if (index >= CHAIN_4RELAY_CHANNEL_COUNT || level > 1) {
        return CHAIN_PARAMETER_ERROR;
    }
    return send4RelayRequest(id, (uint8_t)((index << 4U) | level), operationStatus, timeout);
}

chain_status_t Chain4Relay::set4RelayLevels(uint8_t id, uint8_t levels, uint8_t *operationStatus, unsigned long timeout)
{
    if (levels > 0x0FU) {
        return CHAIN_PARAMETER_ERROR;
    }
    return send4RelayRequest(id, (uint8_t)(0xF0U | levels), operationStatus, timeout);
}

chain_status_t Chain4Relay::get4RelayLevels(uint8_t id, uint8_t *levels, unsigned long timeout)
{
    if (id == 0U || levels == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    chain_status_t status = CHAIN_OK;
    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_4RELAY_GET_LEVEL, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_4RELAY_GET_LEVEL, timeout)) {
            if (returnPacketSize == 10U && checkPacket(returnPacket, returnPacketSize)) {
                *levels = (uint8_t)(returnPacket[6] & 0x0FU);
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

uint16_t Chain4Relay::get4RelayTypeCode(void)
{
    return CHAIN_4RELAY_TYPE_CODE;
}

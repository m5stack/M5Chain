/*
 *SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#include "ChainKey.hpp"

chain_status_t ChainKey::getKeyButtonStatus(uint8_t id, uint8_t *buttonStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_BUTTON_GET_STATUS, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_BUTTON_GET_STATUS, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *buttonStatus = returnPacket[6];
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

chain_status_t ChainKey::setKeyButtonTriggerInterval(uint8_t id, button_double_click_time_t doubleClickInterval,
                                                     button_long_press_time_t longPressInterval,
                                                     uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = doubleClickInterval;
        cmdBuffer[cmdBufferSize++] = longPressInterval;
        sendPacket(id, CHAIN_BUTTON_SET_TRIGGER_TIMEOUT, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_BUTTON_SET_TRIGGER_TIMEOUT, timeout)) {
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
chain_status_t ChainKey::getKeyButtonTriggerInterval(uint8_t id, button_double_click_time_t *doubleClickIntervalMs,
                                                     button_long_press_time_t *longPressIntervalMs,
                                                     unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_BUTTON_GET_TRIGGER_TIMEOUT, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_BUTTON_GET_TRIGGER_TIMEOUT, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *doubleClickIntervalMs = (button_double_click_time_t)returnPacket[6];
                *longPressIntervalMs   = (button_long_press_time_t)returnPacket[7];
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

chain_status_t ChainKey::setKeyButtonMode(uint8_t id, chain_button_mode_t mode, uint8_t *operationStatus,
                                          unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = mode;
        sendPacket(id, CHAIN_BUTTON_SET_MODE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_BUTTON_SET_MODE, timeout)) {
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

chain_status_t ChainKey::getKeyButtonMode(uint8_t id, chain_button_mode_t *mode, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_BUTTON_GET_MODE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_BUTTON_GET_MODE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *mode = (chain_button_mode_t)returnPacket[6];
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

bool ChainKey::getKeyButtonPressStatus(uint8_t id, chain_button_press_type_t *keyPressType)
{
    processIncomingData();
    bool findStatus = 0;
    record_info_t result;
    findStatus = findRecord(&recordList, id, &result);
    if (findStatus == true) {
        *keyPressType = (chain_button_press_type_t)result.type;
        return true;
    }
    return false;
}
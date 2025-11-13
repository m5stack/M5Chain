/*
 *SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#include "ChainEncoder/ChainEncoder.hpp"

chain_status_t ChainEncoder::getEncoderValue(uint8_t id, int16_t *value, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_ENCODER_GET_VALUE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_ENCODER_GET_VALUE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *value = (int16_t)((returnPacket[7] << 8) | returnPacket[6]);
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

chain_status_t ChainEncoder::getEncoderIncValue(uint8_t id, int16_t *incValue, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_ENCODER_GET_INC_VALUE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_ENCODER_GET_INC_VALUE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *incValue = (int16_t)((returnPacket[7] << 8) | returnPacket[6]);
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

chain_status_t ChainEncoder::resetEncoderValue(uint8_t id, uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_ENCODER_RESET_VALUE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_ENCODER_RESET_VALUE, timeout)) {
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

chain_status_t ChainEncoder::resetEncoderIncValue(uint8_t id, uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_ENCODER_RESET_INC_VALUE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_ENCODER_RESET_INC_VALUE, timeout)) {
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

chain_status_t ChainEncoder::setEncoderABDirect(uint8_t id, encoder_ab_t direct, uint8_t *operationStatus,
                                                chain_save_flash_t saveToFlash, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = direct;
        cmdBuffer[cmdBufferSize++] = saveToFlash;
        sendPacket(id, CHAIN_ENCODER_SET_AB_STATUS, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_ENCODER_SET_AB_STATUS, timeout)) {
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

chain_status_t ChainEncoder::getEncoderABDirect(uint8_t id, encoder_ab_t *direct, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_ENCODER_GET_AB_STATUS, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_ENCODER_GET_AB_STATUS, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *direct = (encoder_ab_t)returnPacket[6];
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

chain_status_t ChainEncoder::getEncoderButtonStatus(uint8_t id, uint8_t *buttonStatus, unsigned long timeout)
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

chain_status_t ChainEncoder::setEncoderButtonTriggerInterval(uint8_t id, button_double_click_time_t doubleClickTimeout,
                                                             button_long_press_time_t longPressTimeout,
                                                             uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = doubleClickTimeout;
        cmdBuffer[cmdBufferSize++] = longPressTimeout;
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

chain_status_t ChainEncoder::getEncoderButtonTriggerInterval(uint8_t id, button_double_click_time_t *doubleClickTimeout,
                                                             button_long_press_time_t *longPressTimeout,
                                                             unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_BUTTON_GET_TRIGGER_TIMEOUT, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_BUTTON_GET_TRIGGER_TIMEOUT, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *doubleClickTimeout = (button_double_click_time_t)returnPacket[6];
                *longPressTimeout   = (button_long_press_time_t)returnPacket[7];
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

chain_status_t ChainEncoder::setEncoderButtonMode(uint8_t id, chain_button_mode_t mode, uint8_t *operationStatus,
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

chain_status_t ChainEncoder::getEncoderButtonMode(uint8_t id, chain_button_mode_t *mode, unsigned long timeout)
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

bool ChainEncoder::getEncoderButtonPressStatus(uint8_t id, chain_button_press_type_t *buttonStatus)
{
    processIncomingData();
    bool findStatus = 0;
    record_info_t result;
    findStatus = findRecord(&recordList, id, &result);
    if (findStatus == true) {
        *buttonStatus = (chain_button_press_type_t)result.type;
        return true;
    }
    return false;
}
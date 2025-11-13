/*
 *SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#include "ChainAngle/ChainAngle.hpp"

chain_status_t ChainAngle::getAngle12BitAdc(uint8_t id, uint16_t *adcValue, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_ANGLE_GET_12ADC, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_ANGLE_GET_12ADC, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *adcValue = (returnPacket[7] << 8) | returnPacket[6];
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

chain_status_t ChainAngle::getAngle8BitAdc(uint8_t id, uint8_t *adcValue, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_ANGLE_GET_8ADC, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_ANGLE_GET_8ADC, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *adcValue = returnPacket[6];
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

chain_status_t ChainAngle::setAngleRotationDirection(uint8_t id, angle_rotation_direction_t direction,
                                                     uint8_t *operationStatus, chain_save_flash_t saveToFlash,
                                                     unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (direction > ANGLE_ROTATION_INCREASING) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = direction;
        cmdBuffer[cmdBufferSize++] = saveToFlash;
        sendPacket(id, CHAIN_ANGLE_SET_CLOCKWISE_STATUS, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_ANGLE_SET_CLOCKWISE_STATUS, timeout)) {
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

chain_status_t ChainAngle::getAngleRotationDirection(uint8_t id, angle_rotation_direction_t *direction,
                                                     unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_ANGLE_GET_CLOCKWISE_STATUS, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_ANGLE_GET_CLOCKWISE_STATUS, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *direction = (angle_rotation_direction_t)returnPacket[6];
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
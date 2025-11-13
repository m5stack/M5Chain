/*
 *SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#include "ChainToF/ChainToF.hpp"

chain_status_t ChainToF::getToFDistance(uint8_t id, uint16_t *distance, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_TOF_GET_DISTANCE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_TOF_GET_DISTANCE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *distance = (returnPacket[7] << 8) | returnPacket[6];
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

chain_status_t ChainToF::setToFMeasureTime(uint8_t id, uint8_t time, uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (time < TOF_MEASUREMENT_TIME_MIN || time > TOF_MEASUREMENT_TIME_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = time;
        sendPacket(id, CHAIN_TOF_SET_MEASURE_TIME, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_TOF_SET_MEASURE_TIME, timeout)) {
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

chain_status_t ChainToF::getToFMeasureTime(uint8_t id, uint8_t *time, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_TOF_GET_MEASURE_TIME, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_TOF_GET_MEASURE_TIME, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *time = returnPacket[6];
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

chain_status_t ChainToF::setToFMeasureMode(uint8_t id, chain_tof_mode_t mode, uint8_t *operationStatus,
                                           unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (mode < CHAIN_TOF_MODE_STOP || mode > CHAIN_TOF_MODE_CONTINUOUS) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = mode;
        sendPacket(id, CHAIN_TOF_SET_MEASURE_MODE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_TOF_SET_MEASURE_MODE, timeout)) {
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

chain_status_t ChainToF::getToFMeasureMode(uint8_t id, chain_tof_mode_t *mode, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_TOF_GET_MEASURE_MODE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_TOF_GET_MEASURE_MODE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *mode = (chain_tof_mode_t)returnPacket[6];
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

chain_status_t ChainToF::setToFMeasureStatus(uint8_t id, chain_tof_measure_status_t measureStatus,
                                             uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (measureStatus < CHAIN_TOF_STATUS_STOP || measureStatus > CHAIN_TOF_STATUS_START) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = measureStatus;
        sendPacket(id, CHAIN_TOF_SET_MEASURE_STATUS, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_TOF_SET_MEASURE_STATUS, timeout)) {
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

chain_status_t ChainToF::getToFMeasureStatus(uint8_t id, chain_tof_measure_status_t *measureStatus,
                                             unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_TOF_GET_MEASURE_STATUS, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_TOF_GET_MEASURE_STATUS, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *measureStatus = (chain_tof_measure_status_t)returnPacket[6];
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

chain_status_t ChainToF::getToFMeasureCompleteFlag(uint8_t id, uint8_t *completeFlag, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_TOF_GET_MEASURE_COMPLETE_FLAG, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_TOF_GET_MEASURE_COMPLETE_FLAG, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *completeFlag = returnPacket[6];
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
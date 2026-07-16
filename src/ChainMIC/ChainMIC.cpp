/*
 *SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#include "ChainMIC/ChainMIC.hpp"

chain_status_t ChainMIC::getMIC12BitAdc(uint8_t id, uint16_t *adcValue, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_MIC_GET_12ADC, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_MIC_GET_12ADC, timeout)) {
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

chain_status_t ChainMIC::getMIC8BitAdc(uint8_t id, uint8_t *adcValue, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_MIC_GET_8ADC, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_MIC_GET_8ADC, timeout)) {
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

chain_status_t ChainMIC::setMICThresholdValue(uint8_t id, uint16_t thresholdValue, uint8_t *operationStatus,
                                              chain_save_flash_t saveToFlash, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (thresholdValue > MIC_THRESHOLD_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = thresholdValue & 0xFF;
        cmdBuffer[cmdBufferSize++] = (thresholdValue >> 8) & 0xFF;
        cmdBuffer[cmdBufferSize++] = saveToFlash;
        sendPacket(id, CHAIN_MIC_SET_THRESHOLD_VALUE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_MIC_SET_THRESHOLD_VALUE, timeout)) {
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

chain_status_t ChainMIC::getMICThresholdValue(uint8_t id, uint16_t *thresholdValue, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_MIC_GET_THRESHOLD_VALUE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_MIC_GET_THRESHOLD_VALUE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *thresholdValue = (returnPacket[7] << 8) | returnPacket[6];
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

chain_status_t ChainMIC::setMICReportMode(uint8_t id, chain_mic_report_mode_t mode, uint8_t *operationStatus,
                                          unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = mode;
        sendPacket(id, CHAIN_MIC_SET_REPORT_MODE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_MIC_SET_REPORT_MODE, timeout)) {
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

chain_status_t ChainMIC::getMICReportMode(uint8_t id, chain_mic_report_mode_t *mode, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_MIC_GET_REPORT_MODE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_MIC_GET_REPORT_MODE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *mode = (chain_mic_report_mode_t)returnPacket[6];
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

chain_status_t ChainMIC::setMICTriggerInterval(uint8_t id, uint16_t triggerCycle, uint8_t *operationStatus,
                                               unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (triggerCycle > MIC_TRIGGER_INTERVAL_MAX || triggerCycle < MIC_TRIGGER_INTERVAL_MIN) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = triggerCycle & 0xFF;
        cmdBuffer[cmdBufferSize++] = (triggerCycle >> 8) & 0xFF;
        sendPacket(id, CHAIN_MIC_SET_TRIGGER_INTERVAL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_MIC_SET_TRIGGER_INTERVAL, timeout)) {
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

chain_status_t ChainMIC::getMICTriggerInterval(uint8_t id, uint16_t *triggerCycle, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_MIC_GET_TRIGGER_INTERVAL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_MIC_GET_TRIGGER_INTERVAL, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *triggerCycle = (returnPacket[7] << 8) | returnPacket[6];
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

bool ChainMIC::getMICTriggerStatus(uint8_t id, chain_mic_trigger_t *triggerStatus)
{
    processIncomingData();
    bool findStatus = 0;
    record_info_t result;
    findStatus = findRecord(&recordList, id, &result);
    if (findStatus == true) {
        *triggerStatus = (chain_mic_trigger_t)result.type;
        return true;
    }
    return false;
}

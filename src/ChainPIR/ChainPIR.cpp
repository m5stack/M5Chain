/*
 *SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#include "ChainPIR/ChainPIR.hpp"
chain_status_t ChainPIR::getIRStatus(uint8_t id, pir_detect_result_t *ir_status, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (ir_status == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        memset(cmdBuffer, 0, cmdBufferSize);
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_PIR_GET_IR_STATUS, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_PIR_GET_IR_STATUS, timeout)) {
            if (returnPacketSize >= 7 &&
                checkPacket(reinterpret_cast<const uint8_t *>(returnPacket), returnPacketSize)) {
                *ir_status = (pir_detect_result_t)returnPacket[6];
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

chain_status_t ChainPIR::setPIRDetectTriggerMode(uint8_t id, chain_detect_mode_t auto_status, uint8_t *operationStatus,
                                                 unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (auto_status != CHAIN_DETECT_NONE_REPORT_MODE && auto_status != CHAIN_DETECT_REPORT_MODE) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = static_cast<uint8_t>(auto_status);
        sendPacket(id, CHAIN_PIR_SET_AUTO_SEND_IR_STATUS, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_PIR_SET_AUTO_SEND_IR_STATUS, timeout)) {
            if (returnPacketSize >= 7 &&
                checkPacket(reinterpret_cast<const uint8_t *>(returnPacket), returnPacketSize)) {
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

chain_status_t ChainPIR::getPIRDetectTriggerMode(uint8_t id, chain_detect_mode_t *auto_status, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (auto_status == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        memset(cmdBuffer, 0, cmdBufferSize);
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_PIR_GET_AUTO_SEND_IR_STATUS, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_PIR_GET_AUTO_SEND_IR_STATUS, timeout)) {
            if (returnPacketSize >= 7 &&
                checkPacket(reinterpret_cast<const uint8_t *>(returnPacket), returnPacketSize)) {
                *auto_status = (chain_detect_mode_t)returnPacket[6];
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
bool ChainPIR::getPIRDetectTrigger(uint8_t id, pir_detect_report_t *triggerStatus)
{
    if (triggerStatus == nullptr) {
        return false;
    }

    if (!acquireMutex()) {
        return false;
    }

    processIncomingData();
    bool findStatus = 0;
    record_info_t result;
    findStatus = findRecord(&recordList, id, &result);
    if (findStatus == true &&
        (result.type == CHAIN_PIR_REPORT_PERSON_LEAVE || result.type == CHAIN_PIR_REPORT_PERSON_COME)) {
        *triggerStatus = (pir_detect_report_t)result.type;
        releaseMutex();
        return true;
    }
    releaseMutex();
    return false;
}

chain_status_t ChainPIR::setPIRComeTriggerKeepSeconds(uint8_t id, uint8_t keepSeconds, uint8_t *operationStatus,
                                                      uint8_t saveToFlash, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr ||
        (saveToFlash != CHAIN_SAVE_FLASH_DISABLE && saveToFlash != CHAIN_SAVE_FLASH_ENABLE)) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        memset(cmdBuffer, 0, cmdBufferSize);
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = keepSeconds;
        cmdBuffer[cmdBufferSize++] = saveToFlash;
        sendPacket(id, CHAIN_SET_TRIGGER_KEEP_SECONDS, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_SET_TRIGGER_KEEP_SECONDS, timeout)) {
            if (returnPacketSize >= 7 &&
                checkPacket(reinterpret_cast<const uint8_t *>(returnPacket), returnPacketSize)) {
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

chain_status_t ChainPIR::getPIRComeTriggerKeepSeconds(uint8_t id, uint8_t *keepSeconds, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (keepSeconds == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        memset(cmdBuffer, 0, cmdBufferSize);
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_GET_TRIGGER_KEEP_SECONDS, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_GET_TRIGGER_KEEP_SECONDS, timeout)) {
            if (returnPacketSize >= 7 &&
                checkPacket(reinterpret_cast<const uint8_t *>(returnPacket), returnPacketSize)) {
                *keepSeconds = returnPacket[6];
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

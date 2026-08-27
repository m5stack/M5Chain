/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#include "ChainPedal/ChainPedal.hpp"

chain_status_t ChainPedal::getPedalButtonStatus(uint8_t id, uint8_t *buttonStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (buttonStatus == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_PEDAL_GET_BUTTON_STATUS, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_PEDAL_GET_BUTTON_STATUS, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 10) {
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

chain_status_t ChainPedal::getPedalSwitchStatus(uint8_t id, chain_pedal_switch_status_t *switchStatus,
                                                unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (switchStatus == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_PEDAL_GET_SWITCH_STATUS, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_PEDAL_GET_SWITCH_STATUS, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 10) {
                *switchStatus = (chain_pedal_switch_status_t)returnPacket[6];
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

chain_status_t ChainPedal::setPedalWorkMode(uint8_t id, chain_pedal_work_mode_t workMode, uint8_t *operationStatus,
                                            chain_save_flash_t saveToFlash, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || (workMode != CHAIN_PEDAL_SINGLE_MODE && workMode != CHAIN_PEDAL_SWITCH_MODE) ||
        (saveToFlash != CHAIN_SAVE_FLASH_DISABLE && saveToFlash != CHAIN_SAVE_FLASH_ENABLE)) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = workMode;
        cmdBuffer[cmdBufferSize++] = saveToFlash;
        sendPacket(id, CHAIN_PEDAL_SET_WORK_MODE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_PEDAL_SET_WORK_MODE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 10) {
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
chain_status_t ChainPedal::getPedalWorkMode(uint8_t id, chain_pedal_work_mode_t *workMode, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (workMode == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_PEDAL_GET_WORK_MODE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_PEDAL_GET_WORK_MODE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 10) {
                *workMode = (chain_pedal_work_mode_t)returnPacket[6];
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

chain_status_t ChainPedal::setPedalButtonMode(uint8_t id, chain_button_mode_t buttonMode, uint8_t *operationStatus,
                                              unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr ||
        (buttonMode != CHAIN_BUTTON_NONE_REPORT_MODE && buttonMode != CHAIN_BUTTON_REPORT_MODE)) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)buttonMode;
        sendPacket(id, CHAIN_PEDAL_SET_BUTTON_MODE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_PEDAL_SET_BUTTON_MODE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 10) {
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

chain_status_t ChainPedal::getPedalButtonMode(uint8_t id, chain_button_mode_t *buttonMode, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (buttonMode == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_PEDAL_GET_BUTTON_MODE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_PEDAL_GET_BUTTON_MODE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 10) {
                *buttonMode = (chain_button_mode_t)returnPacket[6];
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

chain_status_t ChainPedal::setPedalEnterConfigTime(uint8_t id, uint8_t enterConfigTime, uint8_t *operationStatus,
                                                   chain_save_flash_t saveToFlash, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || enterConfigTime > PEDAL_ENTER_CONFIG_TIME_MAX ||
        (saveToFlash != CHAIN_SAVE_FLASH_DISABLE && saveToFlash != CHAIN_SAVE_FLASH_ENABLE)) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = enterConfigTime;
        cmdBuffer[cmdBufferSize++] = saveToFlash;
        sendPacket(id, CHAIN_PEDAL_SET_ENTER_CONFIG_TIME, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_PEDAL_SET_ENTER_CONFIG_TIME, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 10) {
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

chain_status_t ChainPedal::getPedalEnterConfigTime(uint8_t id, uint8_t *enterConfigTime, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (enterConfigTime == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_PEDAL_GET_ENTER_CONFIG_TIME, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_PEDAL_GET_ENTER_CONFIG_TIME, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 10) {
                *enterConfigTime = returnPacket[6];
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

chain_status_t ChainPedal::setPedalRGBOverlayValue(uint8_t id, uint8_t overlayValue, uint8_t *operationStatus,
                                                   chain_save_flash_t saveToFlash, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || overlayValue > PEDAL_OVERLAY_VALUE_MAX ||
        (saveToFlash != CHAIN_SAVE_FLASH_DISABLE && saveToFlash != CHAIN_SAVE_FLASH_ENABLE)) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = overlayValue;
        cmdBuffer[cmdBufferSize++] = saveToFlash;
        sendPacket(id, CHAIN_PEDAL_SET_RGB_OVERLAY_VALUE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_PEDAL_SET_RGB_OVERLAY_VALUE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 10) {
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
chain_status_t ChainPedal::getPedalRGBOverlayValue(uint8_t id, uint8_t *overlayValue, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (overlayValue == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_PEDAL_GET_RGB_OVERLAY_VALUE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_PEDAL_GET_RGB_OVERLAY_VALUE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 10) {
                *overlayValue = returnPacket[6];
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

bool ChainPedal::getPedalTriggerStatus(uint8_t id, chain_pedal_press_event_t *triggerStatus)
{
    if (triggerStatus == nullptr) {
        return false;
    }

    processIncomingData();
    bool findStatus = 0;
    record_info_t result;
    findStatus = findRecord(&recordList, id, &result);
    if (findStatus == true) {
        *triggerStatus = (chain_pedal_press_event_t)result.type;
        return true;
    }
    return false;
}

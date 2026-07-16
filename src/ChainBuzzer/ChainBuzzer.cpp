/*
 *SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#include "ChainBuzzer/ChainBuzzer.hpp"

chain_status_t ChainBuzzer::setBuzzerMode(uint8_t id, buzzer_mode_t mode, uint8_t *operationStatus,
                                          unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)mode;
        sendPacket(id, CHAIN_BUZZER_SET_MODE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_BUZZER_SET_MODE, timeout)) {
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

chain_status_t ChainBuzzer::getBuzzerMode(uint8_t id, buzzer_mode_t *mode, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_BUZZER_GET_MODE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_BUZZER_GET_MODE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *mode = (buzzer_mode_t)returnPacket[6];
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

chain_status_t ChainBuzzer::setBuzzerAutoPlay(uint8_t id, uint16_t freq, uint8_t duty, uint16_t duration,
                                              uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (freq < BUZZER_FREQ_MIN || freq > BUZZER_FREQ_MAX || duty > BUZZER_DUTY_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)(freq & 0xFF);
        cmdBuffer[cmdBufferSize++] = (uint8_t)(freq >> 8);
        cmdBuffer[cmdBufferSize++] = duty;
        cmdBuffer[cmdBufferSize++] = (uint8_t)(duration & 0xFF);
        cmdBuffer[cmdBufferSize++] = (uint8_t)(duration >> 8);
        sendPacket(id, CHAIN_BUZZER_SET_AUTO_PLAY, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_BUZZER_SET_AUTO_PLAY, timeout)) {
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

chain_status_t ChainBuzzer::setBuzzerFreq(uint8_t id, uint16_t freq, uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (freq < BUZZER_FREQ_MIN || freq > BUZZER_FREQ_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)(freq & 0xFF);
        cmdBuffer[cmdBufferSize++] = (uint8_t)(freq >> 8);
        sendPacket(id, CHAIN_BUZZER_SET_FREQ, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_BUZZER_SET_FREQ, timeout)) {
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

chain_status_t ChainBuzzer::getBuzzerFreq(uint8_t id, uint16_t *freq, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_BUZZER_GET_FREQ, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_BUZZER_GET_FREQ, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *freq = (returnPacket[7] << 8) | returnPacket[6];
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

chain_status_t ChainBuzzer::setBuzzerDuty(uint8_t id, uint8_t duty, uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (duty > BUZZER_DUTY_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = duty;
        sendPacket(id, CHAIN_BUZZER_SET_DUTY, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_BUZZER_SET_DUTY, timeout)) {
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

chain_status_t ChainBuzzer::getBuzzerDuty(uint8_t id, uint8_t *duty, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_BUZZER_GET_DUTY, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_BUZZER_GET_DUTY, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *duty = returnPacket[6];
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

chain_status_t ChainBuzzer::setBuzzerStatus(uint8_t id, buzzer_status_t buzzerStatus, uint8_t *operationStatus,
                                            unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)buzzerStatus;
        sendPacket(id, CHAIN_BUZZER_SET_STATUS, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_BUZZER_SET_STATUS, timeout)) {
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

chain_status_t ChainBuzzer::getBuzzerStatus(uint8_t id, buzzer_status_t *buzzerStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_BUZZER_GET_STATUS, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_BUZZER_GET_STATUS, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *buzzerStatus = (buzzer_status_t)returnPacket[6];
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

chain_status_t ChainBuzzer::setBuzzerNotePlay(uint8_t id, note_index_t note, uint16_t duration,
                                              uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (note >= NOTE_COUNT) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)note;
        cmdBuffer[cmdBufferSize++] = (uint8_t)(duration & 0xFF);
        cmdBuffer[cmdBufferSize++] = (uint8_t)(duration >> 8);
        sendPacket(id, CHAIN_BUZZER_SET_NOTE_PLAY, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_BUZZER_SET_NOTE_PLAY, timeout)) {
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

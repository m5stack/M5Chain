/*
 *SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#include "ChainDLight.hpp"

static const uint32_t CHAIN_DLIGHT_LUX_THRESHOLD_MAX = 2516580;

static uint16_t readU16LE(const uint8_t *data)
{
    return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

static uint32_t readU32LE(const uint8_t *data)
{
    return (uint32_t)data[0] | ((uint32_t)data[1] << 8) | ((uint32_t)data[2] << 16) | ((uint32_t)data[3] << 24);
}

static void writeU16LE(uint8_t *data, uint16_t value)
{
    data[0] = value & 0xFF;
    data[1] = (value >> 8) & 0xFF;
}

static void writeU32LE(uint8_t *data, uint32_t value)
{
    data[0] = value & 0xFF;
    data[1] = (value >> 8) & 0xFF;
    data[2] = (value >> 16) & 0xFF;
    data[3] = (value >> 24) & 0xFF;
}

static bool isValidEnable(chain_dlight_enable_t enable)
{
    return enable == CHAIN_DLIGHT_DISABLE || enable == CHAIN_DLIGHT_ENABLE;
}

static bool isValidGain(chain_dlight_gain_t gain)
{
    return gain >= CHAIN_DLIGHT_GAIN_1X && gain <= CHAIN_DLIGHT_GAIN_18X;
}

static bool isValidResolution(chain_dlight_resolution_t resolution)
{
    return resolution >= CHAIN_DLIGHT_RESOLUTION_20BIT_400MS && resolution <= CHAIN_DLIGHT_RESOLUTION_16BIT_25MS;
}

static bool isValidRate(chain_dlight_rate_t rate)
{
    return rate >= CHAIN_DLIGHT_RATE_25MS && rate <= CHAIN_DLIGHT_RATE_2000MS;
}

static bool isValidRegLength(chain_dlight_i2c_reg_len_t regLength)
{
    return regLength == CHAIN_DLIGHT_I2C_MEMADD_SIZE_8BIT || regLength == CHAIN_DLIGHT_I2C_MEMADD_SIZE_16BIT;
}

chain_status_t ChainDLight::getDLightLux(uint8_t id, uint32_t *lux, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (lux == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_DLIGHT_GET_LUX, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_DLIGHT_GET_LUX, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 13) {
                *lux = readU32LE(&returnPacket[6]);
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

chain_status_t ChainDLight::setDLightLuxInterruptEnable(uint8_t id, chain_dlight_enable_t enable,
                                                        uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || !isValidEnable(enable)) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)enable;
        sendPacket(id, CHAIN_DLIGHT_SET_LUX_INTERRUPT_ENABLE, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_DLIGHT_SET_LUX_INTERRUPT_ENABLE, timeout)) {
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

chain_status_t ChainDLight::getDLightLuxInterruptEnable(uint8_t id, chain_dlight_enable_t *enable,
                                                        unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (enable == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_DLIGHT_GET_LUX_INTERRUPT_ENABLE, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_DLIGHT_GET_LUX_INTERRUPT_ENABLE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 10) {
                *enable = (chain_dlight_enable_t)returnPacket[6];
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

chain_status_t ChainDLight::setDLightLuxInterruptThreshold(uint8_t id, uint32_t highBoundThreshold,
                                                           uint32_t lowBoundThreshold, uint8_t *operationStatus,
                                                           unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || highBoundThreshold <= lowBoundThreshold ||
        highBoundThreshold > CHAIN_DLIGHT_LUX_THRESHOLD_MAX || lowBoundThreshold > CHAIN_DLIGHT_LUX_THRESHOLD_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        writeU32LE(&cmdBuffer[cmdBufferSize], highBoundThreshold);
        cmdBufferSize += 4;
        writeU32LE(&cmdBuffer[cmdBufferSize], lowBoundThreshold);
        cmdBufferSize += 4;
        sendPacket(id, CHAIN_DLIGHT_SET_LUX_INTERRUPT_THRESHOLD, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_DLIGHT_SET_LUX_INTERRUPT_THRESHOLD, timeout)) {
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

chain_status_t ChainDLight::getDLightLuxInterruptThreshold(uint8_t id, uint32_t *highBoundThreshold,
                                                           uint32_t *lowBoundThreshold, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (highBoundThreshold == nullptr || lowBoundThreshold == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_DLIGHT_GET_LUX_INTERRUPT_THRESHOLD, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_DLIGHT_GET_LUX_INTERRUPT_THRESHOLD, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 17) {
                *highBoundThreshold = readU32LE(&returnPacket[6]);
                *lowBoundThreshold  = readU32LE(&returnPacket[10]);
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

chain_status_t ChainDLight::setDLightSensorGain(uint8_t id, chain_dlight_gain_t gain, uint8_t *operationStatus,
                                                unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || !isValidGain(gain)) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)gain;
        sendPacket(id, CHAIN_DLIGHT_SET_SENSOR_GAIN, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_DLIGHT_SET_SENSOR_GAIN, timeout)) {
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

chain_status_t ChainDLight::getDLightSensorGain(uint8_t id, chain_dlight_gain_t *gain, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (gain == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_DLIGHT_GET_SENSOR_GAIN, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_DLIGHT_GET_SENSOR_GAIN, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 10) {
                *gain = (chain_dlight_gain_t)returnPacket[6];
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

chain_status_t ChainDLight::setDLightSensorResolution(uint8_t id, chain_dlight_resolution_t resolution,
                                                      uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || !isValidResolution(resolution)) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)resolution;
        sendPacket(id, CHAIN_DLIGHT_SET_SENSOR_RESOLUTION, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_DLIGHT_SET_SENSOR_RESOLUTION, timeout)) {
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

chain_status_t ChainDLight::getDLightSensorResolution(uint8_t id, chain_dlight_resolution_t *resolution,
                                                      unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (resolution == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_DLIGHT_GET_SENSOR_RESOLUTION, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_DLIGHT_GET_SENSOR_RESOLUTION, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 10) {
                *resolution = (chain_dlight_resolution_t)returnPacket[6];
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

chain_status_t ChainDLight::setDLightSensorRate(uint8_t id, chain_dlight_rate_t rate, uint8_t *operationStatus,
                                                unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || !isValidRate(rate)) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)rate;
        sendPacket(id, CHAIN_DLIGHT_SET_SENSOR_RATE, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_DLIGHT_SET_SENSOR_RATE, timeout)) {
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

chain_status_t ChainDLight::getDLightSensorRate(uint8_t id, chain_dlight_rate_t *rate, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (rate == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_DLIGHT_GET_SENSOR_RATE, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_DLIGHT_GET_SENSOR_RATE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 10) {
                *rate = (chain_dlight_rate_t)returnPacket[6];
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

chain_status_t ChainDLight::dlightI2cMemRead(uint8_t id, uint8_t i2cAddr, uint16_t regAddr,
                                             chain_dlight_i2c_reg_len_t regLength, uint8_t readLength, uint8_t *buffer,
                                             uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (buffer == nullptr || operationStatus == nullptr || readLength == 0 || readLength > CHAIN_DLIGHT_I2C_MAX_SIZE ||
        !isValidRegLength(regLength)) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = i2cAddr;
        cmdBuffer[cmdBufferSize++] = (uint8_t)regLength;
        cmdBuffer[cmdBufferSize++] = regAddr & 0xFF;
        cmdBuffer[cmdBufferSize++] = (regAddr >> 8) & 0xFF;
        cmdBuffer[cmdBufferSize++] = readLength;
        sendPacket(id, CHAIN_DLIGHT_I2C_MEM_READ, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_DLIGHT_I2C_MEM_READ, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
                if (*operationStatus == 1) {
                    if (returnPacketSize < (uint16_t)(10 + readLength)) {
                        status = CHAIN_RETURN_PACKET_ERROR;
                    } else {
                        for (uint8_t i = 0; i < readLength; i++) {
                            buffer[i] = returnPacket[7 + i];
                        }
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

chain_status_t ChainDLight::dlightI2cMemWrite(uint8_t id, uint8_t i2cAddr, uint16_t regAddr,
                                              chain_dlight_i2c_reg_len_t regLength, uint8_t writeLength,
                                              uint8_t *buffer, uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (buffer == nullptr || operationStatus == nullptr || writeLength == 0 ||
        writeLength > CHAIN_DLIGHT_I2C_MAX_SIZE || !isValidRegLength(regLength)) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = i2cAddr;
        cmdBuffer[cmdBufferSize++] = (uint8_t)regLength;
        cmdBuffer[cmdBufferSize++] = regAddr & 0xFF;
        cmdBuffer[cmdBufferSize++] = (regAddr >> 8) & 0xFF;
        cmdBuffer[cmdBufferSize++] = writeLength;
        for (uint8_t i = 0; i < writeLength; i++) {
            cmdBuffer[cmdBufferSize++] = buffer[i];
        }
        sendPacket(id, CHAIN_DLIGHT_I2C_MEM_WRITE, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_DLIGHT_I2C_MEM_WRITE, timeout)) {
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

chain_status_t ChainDLight::setDLightLuxInterruptEventTriggerMode(uint8_t id, chain_dlight_enable_t mode,
                                                                  uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || !isValidEnable(mode)) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)mode;
        sendPacket(id, CHAIN_DLIGHT_SET_INTERRUPT_EVENT_TRIGGER, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_DLIGHT_SET_INTERRUPT_EVENT_TRIGGER, timeout)) {
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

chain_status_t ChainDLight::getDLightLuxInterruptEventTriggerMode(uint8_t id, chain_dlight_enable_t *mode,
                                                                  unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (mode == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_DLIGHT_GET_INTERRUPT_EVENT_TRIGGER, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_DLIGHT_GET_INTERRUPT_EVENT_TRIGGER, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 10) {
                *mode = (chain_dlight_enable_t)returnPacket[6];
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

uint16_t ChainDLight::getDLightTypeCode(void)
{
    return CHAIN_DLIGHT_TYPE_CODE;
}

bool ChainDLight::getDLightLuxTrigger(uint8_t id, chain_dlight_event_trigger_t *triggerStatus)
{
    processIncomingData();
    bool findStatus = 0;
    record_info_t result;
    findStatus = findRecord(&recordList, id, &result);
    if (findStatus == true) {
        *triggerStatus = (chain_dlight_event_trigger_t)result.type;
        return true;
    }
    return false;
}

/*
 *SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#include "ChainENV/ChainENV.hpp"

static int16_t readI16LE(const uint8_t *data)
{
    return (int16_t)((uint16_t)data[0] | ((uint16_t)data[1] << 8));
}

static uint16_t readU16LE(const uint8_t *data)
{
    return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

static int32_t readI32LE(const uint8_t *data)
{
    return (int32_t)((uint32_t)data[0] | ((uint32_t)data[1] << 8) | ((uint32_t)data[2] << 16) |
                     ((uint32_t)data[3] << 24));
}

static void writeU16LE(uint8_t *data, uint16_t value)
{
    data[0] = value & 0xFF;
    data[1] = (value >> 8) & 0xFF;
}

static void writeI32LE(uint8_t *data, int32_t value)
{
    uint32_t raw = (uint32_t)value;
    data[0]      = raw & 0xFF;
    data[1]      = (raw >> 8) & 0xFF;
    data[2]      = (raw >> 16) & 0xFF;
    data[3]      = (raw >> 24) & 0xFF;
}

static bool isValidRegLength(chain_env_i2c_reg_len_t regLength)
{
    return regLength == CHAIN_ENV_I2C_MEMADD_SIZE_8BIT || regLength == CHAIN_ENV_I2C_MEMADD_SIZE_16BIT;
}

static bool isValidEventMode(uint8_t mode)
{
    return mode <= 0x0F;
}

static bool isValidSPA06Rate(chain_env_spa06_rate_t rate)
{
    return rate >= CHAIN_ENV_SPA06_RATE_1HZ && rate <= CHAIN_ENV_SPA06_RATE_200HZ;
}

static bool isValidTemperatureSource(chain_env_temperature_source_t source)
{
    return source == CHAIN_ENV_TEMPERATURE_SOURCE_AHT20 || source == CHAIN_ENV_TEMPERATURE_SOURCE_SPA06;
}

chain_status_t ChainENV::envI2cMemRead(uint8_t id, uint8_t i2cAddr, uint16_t regAddr, chain_env_i2c_reg_len_t regLength,
                                       uint8_t readLength, uint8_t *buffer, uint8_t *operationStatus,
                                       unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (buffer == nullptr || operationStatus == nullptr || readLength == 0 || readLength > CHAIN_ENV_I2C_MAX_SIZE ||
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
        sendPacket(id, CHAIN_ENV_I2C_MEM_READ, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_ENV_I2C_MEM_READ, timeout)) {
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

chain_status_t ChainENV::envI2cMemWrite(uint8_t id, uint8_t i2cAddr, uint16_t regAddr,
                                        chain_env_i2c_reg_len_t regLength, uint8_t writeLength, uint8_t *buffer,
                                        uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (buffer == nullptr || operationStatus == nullptr || writeLength == 0 || writeLength > CHAIN_ENV_I2C_MAX_SIZE ||
        !isValidRegLength(regLength)) {
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
        sendPacket(id, CHAIN_ENV_I2C_MEM_WRITE, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_ENV_I2C_MEM_WRITE, timeout)) {
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

chain_status_t ChainENV::getAHT20Data(uint8_t id, int16_t *temperature, uint8_t *humidity, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (temperature == nullptr || humidity == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_ENV_GET_AHT20_DATA, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_ENV_GET_AHT20_DATA, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 12) {
                *temperature = readI16LE(&returnPacket[6]);
                *humidity    = returnPacket[8];
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

chain_status_t ChainENV::getSPA06Data(uint8_t id, int16_t *temperature, uint16_t *pressure, int32_t *altitude,
                                      unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (temperature == nullptr || pressure == nullptr || altitude == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_ENV_GET_SPA06_DATA, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_ENV_GET_SPA06_DATA, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 17) {
                *temperature = readI16LE(&returnPacket[6]);
                *pressure    = readU16LE(&returnPacket[8]);
                *altitude    = readI32LE(&returnPacket[10]);
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

chain_status_t ChainENV::setSPA06TemperatureRate(uint8_t id, chain_env_spa06_rate_t rate, uint8_t *operationStatus,
                                                 unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || !isValidSPA06Rate(rate)) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)rate;
        sendPacket(id, CHAIN_ENV_SET_SPA06_TEMPERATURE_RATE, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_ENV_SET_SPA06_TEMPERATURE_RATE, timeout)) {
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

chain_status_t ChainENV::getSPA06TemperatureRate(uint8_t id, chain_env_spa06_rate_t *rate, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (rate == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_ENV_GET_SPA06_TEMPERATURE_RATE, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_ENV_GET_SPA06_TEMPERATURE_RATE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 10) {
                *rate = (chain_env_spa06_rate_t)returnPacket[6];
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

chain_status_t ChainENV::setSPA06PressureRate(uint8_t id, chain_env_spa06_rate_t rate, uint8_t *operationStatus,
                                              unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || !isValidSPA06Rate(rate)) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)rate;
        sendPacket(id, CHAIN_ENV_SET_SPA06_PRESSURE_RATE, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_ENV_SET_SPA06_PRESSURE_RATE, timeout)) {
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

chain_status_t ChainENV::getSPA06PressureRate(uint8_t id, chain_env_spa06_rate_t *rate, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (rate == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_ENV_GET_SPA06_PRESSURE_RATE, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_ENV_GET_SPA06_PRESSURE_RATE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 10) {
                *rate = (chain_env_spa06_rate_t)returnPacket[6];
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

chain_status_t ChainENV::setSPA06SeaLevelPressure(uint8_t id, uint16_t pressure, uint8_t *operationStatus,
                                                  unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || pressure == 0) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        writeU16LE(&cmdBuffer[cmdBufferSize], pressure);
        cmdBufferSize += 2;
        sendPacket(id, CHAIN_ENV_SET_SPA06_SEA_LEVEL_PRESSURE, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_ENV_SET_SPA06_SEA_LEVEL_PRESSURE, timeout)) {
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

chain_status_t ChainENV::getSPA06SeaLevelPressure(uint8_t id, uint16_t *pressure, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (pressure == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_ENV_GET_SPA06_SEA_LEVEL_PRESSURE, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_ENV_GET_SPA06_SEA_LEVEL_PRESSURE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 11) {
                *pressure = readU16LE(&returnPacket[6]);
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

chain_status_t ChainENV::setENVChangeEventTriggerMode(uint8_t id, uint8_t mode, uint8_t *operationStatus,
                                                      unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || !isValidEventMode(mode)) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = mode;
        sendPacket(id, CHAIN_ENV_SET_CHANGE_EVENT_TRIGGER, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_ENV_SET_CHANGE_EVENT_TRIGGER, timeout)) {
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

chain_status_t ChainENV::getENVChangeEventTriggerMode(uint8_t id, uint8_t *mode, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (mode == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_ENV_GET_CHANGE_EVENT_TRIGGER, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_ENV_GET_CHANGE_EVENT_TRIGGER, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 10) {
                *mode = returnPacket[6];
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

chain_status_t ChainENV::setENVTemperatureWarningThreshold(uint8_t id, chain_env_temperature_source_t source,
                                                           uint8_t lowThreshold, uint8_t highThreshold,
                                                           uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || !isValidTemperatureSource(source) || lowThreshold >= highThreshold) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)source;
        cmdBuffer[cmdBufferSize++] = lowThreshold;
        cmdBuffer[cmdBufferSize++] = highThreshold;
        sendPacket(id, CHAIN_ENV_SET_TEMPERATURE_WARNING_THRESHOLD, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_ENV_SET_TEMPERATURE_WARNING_THRESHOLD, timeout)) {
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

chain_status_t ChainENV::getENVTemperatureWarningThreshold(uint8_t id, chain_env_temperature_source_t *source,
                                                           uint8_t *lowThreshold, uint8_t *highThreshold,
                                                           unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (source == nullptr || lowThreshold == nullptr || highThreshold == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_ENV_GET_TEMPERATURE_WARNING_THRESHOLD, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_ENV_GET_TEMPERATURE_WARNING_THRESHOLD, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 12) {
                *source        = (chain_env_temperature_source_t)returnPacket[6];
                *lowThreshold  = returnPacket[7];
                *highThreshold = returnPacket[8];
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

chain_status_t ChainENV::setENVHumidityWarningThreshold(uint8_t id, uint8_t lowThreshold, uint8_t highThreshold,
                                                        uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || highThreshold > 100 || lowThreshold >= highThreshold) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = lowThreshold;
        cmdBuffer[cmdBufferSize++] = highThreshold;
        sendPacket(id, CHAIN_ENV_SET_HUMIDITY_WARNING_THRESHOLD, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_ENV_SET_HUMIDITY_WARNING_THRESHOLD, timeout)) {
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

chain_status_t ChainENV::getENVHumidityWarningThreshold(uint8_t id, uint8_t *lowThreshold, uint8_t *highThreshold,
                                                        unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (lowThreshold == nullptr || highThreshold == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_ENV_GET_HUMIDITY_WARNING_THRESHOLD, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_ENV_GET_HUMIDITY_WARNING_THRESHOLD, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 11) {
                *lowThreshold  = returnPacket[6];
                *highThreshold = returnPacket[7];
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

chain_status_t ChainENV::setENVPressureWarningThreshold(uint8_t id, uint16_t lowThreshold, uint16_t highThreshold,
                                                        uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || lowThreshold >= highThreshold) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        writeU16LE(&cmdBuffer[cmdBufferSize], lowThreshold);
        cmdBufferSize += 2;
        writeU16LE(&cmdBuffer[cmdBufferSize], highThreshold);
        cmdBufferSize += 2;
        sendPacket(id, CHAIN_ENV_SET_PRESSURE_WARNING_THRESHOLD, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_ENV_SET_PRESSURE_WARNING_THRESHOLD, timeout)) {
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

chain_status_t ChainENV::getENVPressureWarningThreshold(uint8_t id, uint16_t *lowThreshold, uint16_t *highThreshold,
                                                        unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (lowThreshold == nullptr || highThreshold == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_ENV_GET_PRESSURE_WARNING_THRESHOLD, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_ENV_GET_PRESSURE_WARNING_THRESHOLD, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 13) {
                *lowThreshold  = readU16LE(&returnPacket[6]);
                *highThreshold = readU16LE(&returnPacket[8]);
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

chain_status_t ChainENV::setENVAltitudeWarningThreshold(uint8_t id, int32_t lowThreshold, int32_t highThreshold,
                                                        uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || lowThreshold >= highThreshold) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        writeI32LE(&cmdBuffer[cmdBufferSize], lowThreshold);
        cmdBufferSize += 4;
        writeI32LE(&cmdBuffer[cmdBufferSize], highThreshold);
        cmdBufferSize += 4;
        sendPacket(id, CHAIN_ENV_SET_ALTITUDE_WARNING_THRESHOLD, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_ENV_SET_ALTITUDE_WARNING_THRESHOLD, timeout)) {
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

chain_status_t ChainENV::getENVAltitudeWarningThreshold(uint8_t id, int32_t *lowThreshold, int32_t *highThreshold,
                                                        unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (lowThreshold == nullptr || highThreshold == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_ENV_GET_ALTITUDE_WARNING_THRESHOLD, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_ENV_GET_ALTITUDE_WARNING_THRESHOLD, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 17) {
                *lowThreshold  = readI32LE(&returnPacket[6]);
                *highThreshold = readI32LE(&returnPacket[10]);
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

uint16_t ChainENV::getENVTypeCode(void)
{
    return CHAIN_ENV_TYPE_CODE;
}

bool ChainENV::getENVChangeTrigger(uint8_t id, uint8_t *triggerStatus, unsigned long timeout)
{
    (void)timeout;

    if (triggerStatus == nullptr) {
        return false;
    }

    processIncomingData();
    record_info_t result;
    if (findRecord(&recordList, id, &result)) {
        uint8_t deviceType = (uint8_t)(result.type >> 8);
        if (deviceType == CHAIN_ENV_TYPE_CODE) {
            *triggerStatus = (uint8_t)(result.type & 0xFF);
            return true;
        }
    }
    return false;
}

chain_status_t ChainENV::getTemperatureData(uint8_t id, int16_t *temperature, unsigned long timeout)
{
    uint8_t humidity = 0;
    return getAHT20Data(id, temperature, &humidity, timeout);
}

chain_status_t ChainENV::getHumidityData(uint8_t id, uint8_t *humidity, unsigned long timeout)
{
    int16_t temperature = 0;
    return getAHT20Data(id, &temperature, humidity, timeout);
}

chain_status_t ChainENV::getPressureData(uint8_t id, uint32_t *pressure, unsigned long timeout)
{
    if (pressure == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    int16_t temperature   = 0;
    uint16_t pressureRaw  = 0;
    int32_t altitude      = 0;
    chain_status_t status = getSPA06Data(id, &temperature, &pressureRaw, &altitude, timeout);
    if (status == CHAIN_OK) {
        *pressure = pressureRaw;
    }
    return status;
}

chain_status_t ChainENV::getAltitudeData(uint8_t id, int32_t *altitude, unsigned long timeout)
{
    if (altitude == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    int16_t temperature  = 0;
    uint16_t pressureRaw = 0;
    return getSPA06Data(id, &temperature, &pressureRaw, altitude, timeout);
}

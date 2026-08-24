/*
 *SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#include "ChainIMU/ChainIMU.hpp"

static int16_t readI16LE(const uint8_t *data)
{
    return (int16_t)((uint16_t)data[0] | ((uint16_t)data[1] << 8));
}

static uint16_t readU16LE(const uint8_t *data)
{
    return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

static bool isValidRegLength(chain_imu_i2c_reg_len_t regLength)
{
    return regLength == CHAIN_IMU_I2C_MEMADD_SIZE_8BIT || regLength == CHAIN_IMU_I2C_MEMADD_SIZE_16BIT;
}

static bool isValidAccelRange(chain_imu_accel_range_t range)
{
    return range >= CHAIN_IMU_ACCEL_RANGE_2G && range <= CHAIN_IMU_ACCEL_RANGE_8G;
}

static bool isValidGyroRange(chain_imu_gyro_range_t range)
{
    return range == CHAIN_IMU_GYRO_RANGE_250DPS || range == CHAIN_IMU_GYRO_RANGE_125DPS ||
           range == CHAIN_IMU_GYRO_RANGE_500DPS || range == CHAIN_IMU_GYRO_RANGE_1000DPS ||
           range == CHAIN_IMU_GYRO_RANGE_2000DPS;
}

static bool isValidAccelODR(chain_imu_accel_odr_t odr)
{
    return odr >= CHAIN_IMU_ACCEL_ODR_OFF && odr <= CHAIN_IMU_ACCEL_ODR_1HZ6;
}

static bool isValidGyroODR(chain_imu_gyro_odr_t odr)
{
    return odr >= CHAIN_IMU_GYRO_ODR_OFF && odr <= CHAIN_IMU_GYRO_ODR_6K66HZ;
}

static bool isValidEventMode(uint8_t mode)
{
    return (mode & 0xF8) == 0;
}

chain_status_t ChainIMU::imuI2cMemRead(uint8_t id, uint8_t i2cAddr, uint16_t regAddr, chain_imu_i2c_reg_len_t regLength,
                                       uint8_t readLength, uint8_t *buffer, uint8_t *operationStatus,
                                       unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (buffer == nullptr || operationStatus == nullptr || readLength == 0 || readLength > CHAIN_IMU_I2C_MAX_SIZE ||
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
        sendPacket(id, CHAIN_IMU_I2C_MEM_READ, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_IMU_I2C_MEM_READ, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize >= 10) {
                *operationStatus = returnPacket[6];
                if (*operationStatus == CHAIN_IMU_OPERATION_SUCCESS) {
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

chain_status_t ChainIMU::imuI2cMemWrite(uint8_t id, uint8_t i2cAddr, uint16_t regAddr,
                                        chain_imu_i2c_reg_len_t regLength, uint8_t writeLength, uint8_t *buffer,
                                        uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (buffer == nullptr || operationStatus == nullptr || writeLength == 0 || writeLength > CHAIN_IMU_I2C_MAX_SIZE ||
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
        sendPacket(id, CHAIN_IMU_I2C_MEM_WRITE, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_IMU_I2C_MEM_WRITE, timeout)) {
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

chain_status_t ChainIMU::getIMUAccelData(uint8_t id, int16_t *x, int16_t *y, int16_t *z, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (x == nullptr || y == nullptr || z == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_IMU_GET_ACCEL_DATA, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_IMU_GET_ACCEL_DATA, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 15) {
                *x = readI16LE(&returnPacket[6]);
                *y = readI16LE(&returnPacket[8]);
                *z = readI16LE(&returnPacket[10]);
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

chain_status_t ChainIMU::getIMUGyroData(uint8_t id, int16_t *x, int16_t *y, int16_t *z, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (x == nullptr || y == nullptr || z == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_IMU_GET_GYRO_DATA, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_IMU_GET_GYRO_DATA, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 15) {
                *x = readI16LE(&returnPacket[6]);
                *y = readI16LE(&returnPacket[8]);
                *z = readI16LE(&returnPacket[10]);
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

chain_status_t ChainIMU::getIMUAccelRawData(uint8_t id, int16_t *x, int16_t *y, int16_t *z, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (x == nullptr || y == nullptr || z == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_IMU_GET_ACCEL_RAW_DATA, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_IMU_GET_ACCEL_RAW_DATA, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 15) {
                *x = readI16LE(&returnPacket[6]);
                *y = readI16LE(&returnPacket[8]);
                *z = readI16LE(&returnPacket[10]);
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

chain_status_t ChainIMU::getIMUGyroRawData(uint8_t id, int16_t *x, int16_t *y, int16_t *z, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (x == nullptr || y == nullptr || z == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_IMU_GET_GYRO_RAW_DATA, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_IMU_GET_GYRO_RAW_DATA, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 15) {
                *x = readI16LE(&returnPacket[6]);
                *y = readI16LE(&returnPacket[8]);
                *z = readI16LE(&returnPacket[10]);
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

chain_status_t ChainIMU::getIMUTemperature(uint8_t id, uint16_t *temperature, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (temperature == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_IMU_GET_TEMPERATURE, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_IMU_GET_TEMPERATURE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 11) {
                *temperature = readU16LE(&returnPacket[6]);
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

chain_status_t ChainIMU::setIMUAccelRange(uint8_t id, chain_imu_accel_range_t range, uint8_t *operationStatus,
                                          unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || !isValidAccelRange(range)) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)range;
        sendPacket(id, CHAIN_IMU_SET_ACCEL_RANGE, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_IMU_SET_ACCEL_RANGE, timeout)) {
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

chain_status_t ChainIMU::getIMUAccelRange(uint8_t id, chain_imu_accel_range_t *range, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (range == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_IMU_GET_ACCEL_RANGE, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_IMU_GET_ACCEL_RANGE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 10) {
                *range = (chain_imu_accel_range_t)returnPacket[6];
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

chain_status_t ChainIMU::setIMUGyroRange(uint8_t id, chain_imu_gyro_range_t range, uint8_t *operationStatus,
                                         unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || !isValidGyroRange(range)) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)range;
        sendPacket(id, CHAIN_IMU_SET_GYRO_RANGE, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_IMU_SET_GYRO_RANGE, timeout)) {
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

chain_status_t ChainIMU::getIMUGyroRange(uint8_t id, chain_imu_gyro_range_t *range, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (range == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_IMU_GET_GYRO_RANGE, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_IMU_GET_GYRO_RANGE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 10) {
                *range = (chain_imu_gyro_range_t)returnPacket[6];
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

chain_status_t ChainIMU::setIMUAccelODR(uint8_t id, chain_imu_accel_odr_t odr, uint8_t *operationStatus,
                                        unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || !isValidAccelODR(odr)) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)odr;
        sendPacket(id, CHAIN_IMU_SET_ACCEL_ODR, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_IMU_SET_ACCEL_ODR, timeout)) {
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

chain_status_t ChainIMU::getIMUAccelODR(uint8_t id, chain_imu_accel_odr_t *odr, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (odr == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_IMU_GET_ACCEL_ODR, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_IMU_GET_ACCEL_ODR, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 10) {
                *odr = (chain_imu_accel_odr_t)returnPacket[6];
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

chain_status_t ChainIMU::setIMUGyroODR(uint8_t id, chain_imu_gyro_odr_t odr, uint8_t *operationStatus,
                                       unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || !isValidGyroODR(odr)) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)odr;
        sendPacket(id, CHAIN_IMU_SET_GYRO_ODR, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_IMU_SET_GYRO_ODR, timeout)) {
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

chain_status_t ChainIMU::getIMUGyroODR(uint8_t id, chain_imu_gyro_odr_t *odr, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (odr == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_IMU_GET_GYRO_ODR, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_IMU_GET_GYRO_ODR, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 10) {
                *odr = (chain_imu_gyro_odr_t)returnPacket[6];
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

chain_status_t ChainIMU::setIMUEventTriggerMode(uint8_t id, uint8_t mode, uint8_t *operationStatus,
                                                unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || !isValidEventMode(mode)) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = mode;
        sendPacket(id, CHAIN_IMU_SET_EVENT_TRIGGER, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_IMU_SET_EVENT_TRIGGER, timeout)) {
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

chain_status_t ChainIMU::getIMUEventTriggerMode(uint8_t id, uint8_t *mode, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (mode == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_IMU_GET_EVENT_TRIGGER, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_IMU_GET_EVENT_TRIGGER, timeout)) {
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

chain_status_t ChainIMU::setIMU6AxisWarningThreshold(uint8_t id, uint8_t accelThreshold, uint8_t gyroThreshold,
                                                     uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = accelThreshold;
        cmdBuffer[cmdBufferSize++] = gyroThreshold;
        sendPacket(id, CHAIN_IMU_SET_6AXIS_THRESHOLD, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_IMU_SET_6AXIS_THRESHOLD, timeout)) {
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

chain_status_t ChainIMU::getIMU6AxisWarningThreshold(uint8_t id, uint8_t *accelThreshold, uint8_t *gyroThreshold,
                                                     unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (accelThreshold == nullptr || gyroThreshold == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_IMU_GET_6AXIS_THRESHOLD, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_IMU_GET_6AXIS_THRESHOLD, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize) && returnPacketSize == 11) {
                *accelThreshold = returnPacket[6];
                *gyroThreshold  = returnPacket[7];
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

chain_status_t ChainIMU::setIMUTemperatureWarningThreshold(uint8_t id, uint8_t lowThreshold, uint8_t highThreshold,
                                                           uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (operationStatus == nullptr || lowThreshold >= highThreshold) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = lowThreshold;
        cmdBuffer[cmdBufferSize++] = highThreshold;
        sendPacket(id, CHAIN_IMU_SET_TEMPERATURE_THRESHOLD, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_IMU_SET_TEMPERATURE_THRESHOLD, timeout)) {
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

chain_status_t ChainIMU::getIMUTemperatureWarningThreshold(uint8_t id, uint8_t *lowThreshold, uint8_t *highThreshold,
                                                           unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (lowThreshold == nullptr || highThreshold == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_IMU_GET_TEMPERATURE_THRESHOLD, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_IMU_GET_TEMPERATURE_THRESHOLD, timeout)) {
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

uint16_t ChainIMU::getIMUTypeCode(void)
{
    return CHAIN_IMU_TYPE_CODE;
}

bool ChainIMU::getIMUEventTrigger(uint8_t id, uint8_t *intStatus, uint8_t *reportCode, unsigned long timeout)
{
    (void)timeout;

    if (intStatus == nullptr) {
        return false;
    }

    processIncomingData();
    record_info_t result;
    if (findRecord(&recordList, id, &result)) {
        uint8_t receivedReportCode = (uint8_t)(result.type >> 8);
        if (receivedReportCode == CHAIN_IMU_EVENT_REPORT_CODE) {
            *intStatus = (uint8_t)(result.type & 0xFF);
            if (reportCode != nullptr) {
                *reportCode = receivedReportCode;
            }
            return true;
        }
    }
    return false;
}

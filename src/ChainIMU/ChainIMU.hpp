/*
 *SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#ifndef CHAIN_IMU_HPP_
#define CHAIN_IMU_HPP_

#include "ChainCommon/ChainCommon.hpp"

/**
 * @brief Report code used by IMU event auto-report packets.
 */
#define CHAIN_IMU_EVENT_REPORT_CODE (0x10)

/**
 * @brief Maximum size for IMU I2C read/write operations.
 */
#define CHAIN_IMU_I2C_MAX_SIZE (64)

#define CHAIN_IMU_EVENT_TRIGGER_REPORT_TYPE     (0x1000)
#define CHAIN_IMU_EVENT_TRIGGER_REPORT_MASK     (0xFF00)
#define CHAIN_IMU_EVENT_TRIGGER_INT_STATUS_MASK (0x00FF)

#define CHAIN_IMU_EVENT_TRIGGER_ACCEL_MASK    (0xFF01)
#define CHAIN_IMU_EVENT_TRIGGER_ACCEL_RELEASE (0x1000)
#define CHAIN_IMU_EVENT_TRIGGER_ACCEL_ACTIVE  (0x1001)

#define CHAIN_IMU_EVENT_TRIGGER_GYRO_MASK    (0xFF02)
#define CHAIN_IMU_EVENT_TRIGGER_GYRO_RELEASE (0x1000)
#define CHAIN_IMU_EVENT_TRIGGER_GYRO_ACTIVE  (0x1002)

#define CHAIN_IMU_EVENT_TRIGGER_TEMPERATURE_MASK         (0xFF0C)
#define CHAIN_IMU_EVENT_TRIGGER_TEMPERATURE_HIGH_RELEASE (0x1000)
#define CHAIN_IMU_EVENT_TRIGGER_TEMPERATURE_HIGH_ACTIVE  (0x1004)
#define CHAIN_IMU_EVENT_TRIGGER_TEMPERATURE_LOW_ACTIVE   (0x1008)
#define CHAIN_IMU_EVENT_TRIGGER_TEMPERATURE_LOW_RELEASE  (0x100C)

#define CHAIN_IMU_EVENT_TRIGGER_RESERVED_MASK (0xFFE0)

/**
 * @brief Enumeration for CHAIN_IMU device commands.
 */
typedef enum {
    CHAIN_IMU_I2C_MEM_READ              = 0x13, /**< Read data from specific address on I2C device. */
    CHAIN_IMU_I2C_MEM_WRITE             = 0x14, /**< Write data to specific address on I2C device. */
    CHAIN_IMU_SET_RGB_VALUE             = 0x20, /**< Set RGB value. Inherited from ChainCommon. */
    CHAIN_IMU_GET_RGB_VALUE             = 0x21, /**< Get RGB value. Inherited from ChainCommon. */
    CHAIN_IMU_SET_RGB_LIGHT             = 0x22, /**< Set RGB brightness. Inherited from ChainCommon. */
    CHAIN_IMU_GET_RGB_LIGHT             = 0x23, /**< Get RGB brightness. Inherited from ChainCommon. */
    CHAIN_IMU_GET_ACCEL_DATA            = 0x30, /**< Get accelerometer data in mg. */
    CHAIN_IMU_GET_GYRO_DATA             = 0x31, /**< Get gyroscope data in dps. */
    CHAIN_IMU_GET_ACCEL_RAW_DATA        = 0x32, /**< Get accelerometer raw data. */
    CHAIN_IMU_GET_GYRO_RAW_DATA         = 0x33, /**< Get gyroscope raw data. */
    CHAIN_IMU_GET_TEMPERATURE           = 0x34, /**< Get IMU temperature in Celsius x100. */
    CHAIN_IMU_SET_ACCEL_RANGE           = 0x35, /**< Set accelerometer range. */
    CHAIN_IMU_GET_ACCEL_RANGE           = 0x36, /**< Get accelerometer range. */
    CHAIN_IMU_SET_GYRO_RANGE            = 0x37, /**< Set gyroscope range. */
    CHAIN_IMU_GET_GYRO_RANGE            = 0x38, /**< Get gyroscope range. */
    CHAIN_IMU_SET_ACCEL_ODR             = 0x39, /**< Set accelerometer output data rate. */
    CHAIN_IMU_GET_ACCEL_ODR             = 0x3A, /**< Get accelerometer output data rate. */
    CHAIN_IMU_SET_GYRO_ODR              = 0x3B, /**< Set gyroscope output data rate. */
    CHAIN_IMU_GET_GYRO_ODR              = 0x3C, /**< Get gyroscope output data rate. */
    CHAIN_IMU_REPORT_EVENT_TRIGGER      = 0xE0, /**< IMU event trigger auto-report packet. */
    CHAIN_IMU_SET_EVENT_TRIGGER         = 0xE1, /**< Set IMU event trigger mask. */
    CHAIN_IMU_GET_EVENT_TRIGGER         = 0xE2, /**< Get IMU event trigger mask. */
    CHAIN_IMU_SET_6AXIS_THRESHOLD       = 0xE3, /**< Set accelerometer and gyroscope thresholds. */
    CHAIN_IMU_GET_6AXIS_THRESHOLD       = 0xE4, /**< Get accelerometer and gyroscope thresholds. */
    CHAIN_IMU_SET_TEMPERATURE_THRESHOLD = 0xE5, /**< Set temperature warning thresholds. */
    CHAIN_IMU_GET_TEMPERATURE_THRESHOLD = 0xE6, /**< Get temperature warning thresholds. */
    CHAIN_IMU_GET_UID                   = 0xF8, /**< Get unique identifier. Inherited from ChainCommon. */
    CHAIN_IMU_GET_BOOTLOADER_VERSION    = 0xF9, /**< Get bootloader version. Inherited from ChainCommon. */
    CHAIN_IMU_GET_FIRMWARE_VERSION      = 0xFA, /**< Get firmware version. Inherited from ChainCommon. */
    CHAIN_IMU_GET_DEVICE_TYPE           = 0xFB, /**< Get device type. Inherited from ChainCommon. */
    CHAIN_IMU_ENUM_PLEASE               = 0xFC, /**< Enumeration request notification. */
    CHAIN_IMU_HEARTBEAT                 = 0xFD, /**< Heartbeat packet. */
    CHAIN_IMU_ENUM                      = 0xFE, /**< Enumeration packet. */
} CHAIN_IMU_CMD_T;

/**
 * @brief Chain IMU operation status enumeration.
 */
typedef enum {
    CHAIN_IMU_OPERATION_FAIL    = 0x00, /**< Operation failed. */
    CHAIN_IMU_OPERATION_SUCCESS = 0x01, /**< Operation succeeded. */
} chain_imu_operation_status_t;

/**
 * @brief Chain IMU accelerometer range enumeration.
 */
typedef enum {
    CHAIN_IMU_ACCEL_RANGE_2G  = 0x00,
    CHAIN_IMU_ACCEL_RANGE_16G = 0x01,
    CHAIN_IMU_ACCEL_RANGE_4G  = 0x02,
    CHAIN_IMU_ACCEL_RANGE_8G  = 0x03,
} chain_imu_accel_range_t;

/**
 * @brief Chain IMU gyroscope range enumeration.
 */
typedef enum {
    CHAIN_IMU_GYRO_RANGE_250DPS  = 0x00,
    CHAIN_IMU_GYRO_RANGE_125DPS  = 0x01,
    CHAIN_IMU_GYRO_RANGE_500DPS  = 0x02,
    CHAIN_IMU_GYRO_RANGE_1000DPS = 0x04,
    CHAIN_IMU_GYRO_RANGE_2000DPS = 0x06,
} chain_imu_gyro_range_t;

/**
 * @brief Chain IMU accelerometer output data rate enumeration.
 */
typedef enum {
    CHAIN_IMU_ACCEL_ODR_OFF    = 0x00,
    CHAIN_IMU_ACCEL_ODR_12HZ5  = 0x01,
    CHAIN_IMU_ACCEL_ODR_26HZ   = 0x02,
    CHAIN_IMU_ACCEL_ODR_52HZ   = 0x03,
    CHAIN_IMU_ACCEL_ODR_104HZ  = 0x04,
    CHAIN_IMU_ACCEL_ODR_208HZ  = 0x05,
    CHAIN_IMU_ACCEL_ODR_416HZ  = 0x06,
    CHAIN_IMU_ACCEL_ODR_833HZ  = 0x07,
    CHAIN_IMU_ACCEL_ODR_1K66HZ = 0x08,
    CHAIN_IMU_ACCEL_ODR_3K33HZ = 0x09,
    CHAIN_IMU_ACCEL_ODR_6K66HZ = 0x0A,
    CHAIN_IMU_ACCEL_ODR_1HZ6   = 0x0B,

    CHAIN_IMU_ACCEL_ODR_1660HZ = CHAIN_IMU_ACCEL_ODR_1K66HZ, /**< Legacy alias. */
} chain_imu_accel_odr_t;

/**
 * @brief Chain IMU gyroscope output data rate enumeration.
 */
typedef enum {
    CHAIN_IMU_GYRO_ODR_OFF    = 0x00,
    CHAIN_IMU_GYRO_ODR_12HZ5  = 0x01,
    CHAIN_IMU_GYRO_ODR_26HZ   = 0x02,
    CHAIN_IMU_GYRO_ODR_52HZ   = 0x03,
    CHAIN_IMU_GYRO_ODR_104HZ  = 0x04,
    CHAIN_IMU_GYRO_ODR_208HZ  = 0x05,
    CHAIN_IMU_GYRO_ODR_416HZ  = 0x06,
    CHAIN_IMU_GYRO_ODR_833HZ  = 0x07,
    CHAIN_IMU_GYRO_ODR_1K66HZ = 0x08,
    CHAIN_IMU_GYRO_ODR_3K33HZ = 0x09,
    CHAIN_IMU_GYRO_ODR_6K66HZ = 0x0A,

    CHAIN_IMU_GYRO_ODR_1660HZ = CHAIN_IMU_GYRO_ODR_1K66HZ, /**< Legacy alias. */
} chain_imu_gyro_odr_t;

/**
 * @brief Enumeration for I2C register address length.
 */
typedef enum {
    CHAIN_IMU_I2C_MEMADD_SIZE_8BIT  = 1, /**< 8-bit address length. */
    CHAIN_IMU_I2C_MEMADD_SIZE_16BIT = 2, /**< 16-bit address length. */
} chain_imu_i2c_reg_len_t;

/**
 * @brief Chain IMU event trigger mode bit masks.
 */
typedef enum {
    CHAIN_IMU_EVENT_MODE_ACCEL       = 0x01,
    CHAIN_IMU_EVENT_MODE_GYRO        = 0x02,
    CHAIN_IMU_EVENT_MODE_TEMPERATURE = 0x04,
} chain_imu_event_mode_t;

/**
 * @brief Chain IMU INT_Status bit masks.
 */
#define CHAIN_IMU_EVENT_STATUS_ACCEL_MASK    (0x01)
#define CHAIN_IMU_EVENT_STATUS_ACCEL_RELEASE (0x00)
#define CHAIN_IMU_EVENT_STATUS_ACCEL_ACTIVE  (0x01)

#define CHAIN_IMU_EVENT_STATUS_GYRO_MASK    (0x02)
#define CHAIN_IMU_EVENT_STATUS_GYRO_RELEASE (0x00)
#define CHAIN_IMU_EVENT_STATUS_GYRO_ACTIVE  (0x02)

#define CHAIN_IMU_EVENT_STATUS_TEMPERATURE_MASK         (0x0C)
#define CHAIN_IMU_EVENT_STATUS_TEMPERATURE_SHIFT        (2)
#define CHAIN_IMU_EVENT_STATUS_TEMPERATURE_HIGH_RELEASE (0x00)
#define CHAIN_IMU_EVENT_STATUS_TEMPERATURE_HIGH_ACTIVE  (0x04)
#define CHAIN_IMU_EVENT_STATUS_TEMPERATURE_LOW_ACTIVE   (0x08)
#define CHAIN_IMU_EVENT_STATUS_TEMPERATURE_LOW_RELEASE  (0x0C)

#define CHAIN_IMU_EVENT_STATUS_RESERVED_MASK (0xE0)

class ChainIMU : virtual public ChainCommon {
public:
    /**
     * @brief Reads data from a specific register address of an I2C device through IMU.
     */
    chain_status_t imuI2cMemRead(uint8_t id, uint8_t i2cAddr, uint16_t regAddr, chain_imu_i2c_reg_len_t regLength,
                                 uint8_t readLength, uint8_t *buffer, uint8_t *operationStatus,
                                 unsigned long timeout = 100);

    /**
     * @brief Writes data to a specific register address of an I2C device through IMU.
     */
    chain_status_t imuI2cMemWrite(uint8_t id, uint8_t i2cAddr, uint16_t regAddr, chain_imu_i2c_reg_len_t regLength,
                                  uint8_t writeLength, uint8_t *buffer, uint8_t *operationStatus,
                                  unsigned long timeout = 100);

    /**
     * @brief Gets accelerometer X/Y/Z data in mg.
     */
    chain_status_t getIMUAccelData(uint8_t id, int16_t *x, int16_t *y, int16_t *z, unsigned long timeout = 100);

    /**
     * @brief Gets gyroscope X/Y/Z data in dps.
     */
    chain_status_t getIMUGyroData(uint8_t id, int16_t *x, int16_t *y, int16_t *z, unsigned long timeout = 100);

    /**
     * @brief Gets accelerometer X/Y/Z raw data.
     */
    chain_status_t getIMUAccelRawData(uint8_t id, int16_t *x, int16_t *y, int16_t *z, unsigned long timeout = 100);

    /**
     * @brief Gets gyroscope X/Y/Z raw data.
     */
    chain_status_t getIMUGyroRawData(uint8_t id, int16_t *x, int16_t *y, int16_t *z, unsigned long timeout = 100);

    /**
     * @brief Gets IMU temperature in Celsius x100.
     */
    chain_status_t getIMUTemperature(uint8_t id, uint16_t *temperature, unsigned long timeout = 100);

    /**
     * @brief Sets the accelerometer range.
     */
    chain_status_t setIMUAccelRange(uint8_t id, chain_imu_accel_range_t range, uint8_t *operationStatus,
                                    unsigned long timeout = 100);

    /**
     * @brief Gets the accelerometer range.
     */
    chain_status_t getIMUAccelRange(uint8_t id, chain_imu_accel_range_t *range, unsigned long timeout = 100);

    /**
     * @brief Sets the gyroscope range.
     */
    chain_status_t setIMUGyroRange(uint8_t id, chain_imu_gyro_range_t range, uint8_t *operationStatus,
                                   unsigned long timeout = 100);

    /**
     * @brief Gets the gyroscope range.
     */
    chain_status_t getIMUGyroRange(uint8_t id, chain_imu_gyro_range_t *range, unsigned long timeout = 100);

    /**
     * @brief Sets the accelerometer output data rate.
     */
    chain_status_t setIMUAccelODR(uint8_t id, chain_imu_accel_odr_t odr, uint8_t *operationStatus,
                                  unsigned long timeout = 100);

    /**
     * @brief Gets the accelerometer output data rate.
     */
    chain_status_t getIMUAccelODR(uint8_t id, chain_imu_accel_odr_t *odr, unsigned long timeout = 100);

    /**
     * @brief Sets the gyroscope output data rate.
     */
    chain_status_t setIMUGyroODR(uint8_t id, chain_imu_gyro_odr_t odr, uint8_t *operationStatus,
                                 unsigned long timeout = 100);

    /**
     * @brief Gets the gyroscope output data rate.
     */
    chain_status_t getIMUGyroODR(uint8_t id, chain_imu_gyro_odr_t *odr, unsigned long timeout = 100);

    /**
     * @brief Sets IMU event trigger mode mask (bit0: accel, bit1: gyro, bit2: temperature).
     */
    chain_status_t setIMUEventTriggerMode(uint8_t id, uint8_t mode, uint8_t *operationStatus,
                                          unsigned long timeout = 100);

    /**
     * @brief Gets IMU event trigger mode mask.
     */
    chain_status_t getIMUEventTriggerMode(uint8_t id, uint8_t *mode, unsigned long timeout = 100);

    /**
     * @brief Sets the accelerometer and gyroscope warning thresholds.
     */
    chain_status_t setIMU6AxisWarningThreshold(uint8_t id, uint8_t accelThreshold, uint8_t gyroThreshold,
                                               uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Gets the accelerometer and gyroscope warning thresholds.
     */
    chain_status_t getIMU6AxisWarningThreshold(uint8_t id, uint8_t *accelThreshold, uint8_t *gyroThreshold,
                                               unsigned long timeout = 100);

    /**
     * @brief Sets the temperature warning thresholds in Celsius.
     */
    chain_status_t setIMUTemperatureWarningThreshold(uint8_t id, uint8_t lowThreshold, uint8_t highThreshold,
                                                     uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Gets the temperature warning thresholds in Celsius.
     */
    chain_status_t getIMUTemperatureWarningThreshold(uint8_t id, uint8_t *lowThreshold, uint8_t *highThreshold,
                                                     unsigned long timeout = 100);

    /**
     * @brief Gets the Chain IMU type code.
     */
    uint16_t getIMUTypeCode(void);

    /**
     * @brief Gets the latest IMU event trigger auto-report status.
     *
     * @param intStatus Status byte from the 0xE0 auto-report packet.
     * @param reportCode Optional pointer to store the report code. The protocol defines 0x10.
     *
     * @return true if a trigger record was available.
     */
    bool getIMUEventTrigger(uint8_t id, uint8_t *intStatus, uint8_t *reportCode = nullptr, unsigned long timeout = 100);
};

#endif  // CHAIN_IMU_HPP_

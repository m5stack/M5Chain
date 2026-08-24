/*
 *SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#ifndef CHAIN_ENV_HPP_
#define CHAIN_ENV_HPP_

#include "ChainCommon/ChainCommon.hpp"

/**
 * @brief Maximum size for Chain ENV I2C read/write operations.
 */
#define CHAIN_ENV_I2C_MAX_SIZE (32)

#define CHAIN_ENV_EVENT_TRIGGER_REPORT_TYPE     (0x0F00)
#define CHAIN_ENV_EVENT_TRIGGER_REPORT_MASK     (0xFF00)
#define CHAIN_ENV_EVENT_TRIGGER_INT_STATUS_MASK (0x00FF)

#define CHAIN_ENV_EVENT_TRIGGER_TEMPERATURE_MASK         (0xFF03)
#define CHAIN_ENV_EVENT_TRIGGER_TEMPERATURE_HIGH_RELEASE (0x0F00)
#define CHAIN_ENV_EVENT_TRIGGER_TEMPERATURE_HIGH_ACTIVE  (0x0F01)
#define CHAIN_ENV_EVENT_TRIGGER_TEMPERATURE_LOW_ACTIVE   (0x0F02)
#define CHAIN_ENV_EVENT_TRIGGER_TEMPERATURE_LOW_RELEASE  (0x0F03)

#define CHAIN_ENV_EVENT_TRIGGER_HUMIDITY_MASK         (0xFF0C)
#define CHAIN_ENV_EVENT_TRIGGER_HUMIDITY_HIGH_RELEASE (0x0F00)
#define CHAIN_ENV_EVENT_TRIGGER_HUMIDITY_HIGH_ACTIVE  (0x0F04)
#define CHAIN_ENV_EVENT_TRIGGER_HUMIDITY_LOW_ACTIVE   (0x0F08)
#define CHAIN_ENV_EVENT_TRIGGER_HUMIDITY_LOW_RELEASE  (0x0F0C)

#define CHAIN_ENV_EVENT_TRIGGER_PRESSURE_MASK         (0xFF30)
#define CHAIN_ENV_EVENT_TRIGGER_PRESSURE_HIGH_RELEASE (0x0F00)
#define CHAIN_ENV_EVENT_TRIGGER_PRESSURE_HIGH_ACTIVE  (0x0F10)
#define CHAIN_ENV_EVENT_TRIGGER_PRESSURE_LOW_ACTIVE   (0x0F20)
#define CHAIN_ENV_EVENT_TRIGGER_PRESSURE_LOW_RELEASE  (0x0F30)

#define CHAIN_ENV_EVENT_TRIGGER_ALTITUDE_MASK         (0xFFC0)
#define CHAIN_ENV_EVENT_TRIGGER_ALTITUDE_HIGH_RELEASE (0x0F00)
#define CHAIN_ENV_EVENT_TRIGGER_ALTITUDE_HIGH_ACTIVE  (0x0F40)
#define CHAIN_ENV_EVENT_TRIGGER_ALTITUDE_LOW_ACTIVE   (0x0F80)
#define CHAIN_ENV_EVENT_TRIGGER_ALTITUDE_LOW_RELEASE  (0x0FC0)

#define CHAIN_ENV_EVENT_STATUS_TEMPERATURE_MASK         (0x03)
#define CHAIN_ENV_EVENT_STATUS_TEMPERATURE_SHIFT        (0)
#define CHAIN_ENV_EVENT_STATUS_TEMPERATURE_HIGH_RELEASE (0x00)
#define CHAIN_ENV_EVENT_STATUS_TEMPERATURE_HIGH_ACTIVE  (0x01)
#define CHAIN_ENV_EVENT_STATUS_TEMPERATURE_LOW_ACTIVE   (0x02)
#define CHAIN_ENV_EVENT_STATUS_TEMPERATURE_LOW_RELEASE  (0x03)

#define CHAIN_ENV_EVENT_STATUS_HUMIDITY_MASK         (0x0C)
#define CHAIN_ENV_EVENT_STATUS_HUMIDITY_SHIFT        (2)
#define CHAIN_ENV_EVENT_STATUS_HUMIDITY_HIGH_RELEASE (0x00)
#define CHAIN_ENV_EVENT_STATUS_HUMIDITY_HIGH_ACTIVE  (0x04)
#define CHAIN_ENV_EVENT_STATUS_HUMIDITY_LOW_ACTIVE   (0x08)
#define CHAIN_ENV_EVENT_STATUS_HUMIDITY_LOW_RELEASE  (0x0C)

#define CHAIN_ENV_EVENT_STATUS_PRESSURE_MASK         (0x30)
#define CHAIN_ENV_EVENT_STATUS_PRESSURE_SHIFT        (4)
#define CHAIN_ENV_EVENT_STATUS_PRESSURE_HIGH_RELEASE (0x00)
#define CHAIN_ENV_EVENT_STATUS_PRESSURE_HIGH_ACTIVE  (0x10)
#define CHAIN_ENV_EVENT_STATUS_PRESSURE_LOW_ACTIVE   (0x20)
#define CHAIN_ENV_EVENT_STATUS_PRESSURE_LOW_RELEASE  (0x30)

#define CHAIN_ENV_EVENT_STATUS_ALTITUDE_MASK         (0xC0)
#define CHAIN_ENV_EVENT_STATUS_ALTITUDE_SHIFT        (6)
#define CHAIN_ENV_EVENT_STATUS_ALTITUDE_HIGH_RELEASE (0x00)
#define CHAIN_ENV_EVENT_STATUS_ALTITUDE_HIGH_ACTIVE  (0x40)
#define CHAIN_ENV_EVENT_STATUS_ALTITUDE_LOW_ACTIVE   (0x80)
#define CHAIN_ENV_EVENT_STATUS_ALTITUDE_LOW_RELEASE  (0xC0)

/**
 * @brief Enumeration for CHAIN_ENV device commands.
 *
 * This enumeration defines command codes for various operations of the CHAIN_ENV device.
 */
typedef enum {
    CHAIN_ENV_I2C_MEM_READ                      = 0x13, /**< Read data from specific address on I2C device. */
    CHAIN_ENV_I2C_MEM_WRITE                     = 0x14, /**< Write data to specific address on I2C device. */
    CHAIN_ENV_GET_AHT20_DATA                    = 0x30, /**< Get AHT20 temperature and humidity. */
    CHAIN_ENV_GET_SPA06_DATA                    = 0x31, /**< Get SPA06 temperature, pressure and altitude. */
    CHAIN_ENV_SET_SPA06_TEMPERATURE_RATE        = 0x32, /**< Set SPA06 temperature measurement rate. */
    CHAIN_ENV_GET_SPA06_TEMPERATURE_RATE        = 0x33, /**< Get SPA06 temperature measurement rate. */
    CHAIN_ENV_SET_SPA06_PRESSURE_RATE           = 0x34, /**< Set SPA06 pressure measurement rate. */
    CHAIN_ENV_GET_SPA06_PRESSURE_RATE           = 0x35, /**< Get SPA06 pressure measurement rate. */
    CHAIN_ENV_SET_SPA06_SEA_LEVEL_PRESSURE      = 0x36, /**< Set SPA06 sea-level pressure for altitude calculation. */
    CHAIN_ENV_GET_SPA06_SEA_LEVEL_PRESSURE      = 0x37, /**< Get SPA06 sea-level pressure for altitude calculation. */
    CHAIN_ENV_REPORT_ENV_CHANGE                 = 0xE0, /**< ENV change warning auto-report packet. */
    CHAIN_ENV_SET_CHANGE_EVENT_TRIGGER          = 0xE1, /**< Set ENV change warning auto-report mode. */
    CHAIN_ENV_GET_CHANGE_EVENT_TRIGGER          = 0xE2, /**< Get ENV change warning auto-report mode. */
    CHAIN_ENV_SET_TEMPERATURE_WARNING_THRESHOLD = 0xE3, /**< Set temperature warning thresholds. */
    CHAIN_ENV_GET_TEMPERATURE_WARNING_THRESHOLD = 0xE4, /**< Get temperature warning thresholds. */
    CHAIN_ENV_SET_HUMIDITY_WARNING_THRESHOLD    = 0xE5, /**< Set humidity warning thresholds. */
    CHAIN_ENV_GET_HUMIDITY_WARNING_THRESHOLD    = 0xE6, /**< Get humidity warning thresholds. */
    CHAIN_ENV_SET_PRESSURE_WARNING_THRESHOLD    = 0xE7, /**< Set pressure warning thresholds. */
    CHAIN_ENV_GET_PRESSURE_WARNING_THRESHOLD    = 0xE8, /**< Get pressure warning thresholds. */
    CHAIN_ENV_SET_ALTITUDE_WARNING_THRESHOLD    = 0xE9, /**< Set altitude warning thresholds. */
    CHAIN_ENV_GET_ALTITUDE_WARNING_THRESHOLD    = 0xEA, /**< Get altitude warning thresholds. */

    CHAIN_ENV_GET_TEMPERATURE_DATA = CHAIN_ENV_GET_AHT20_DATA, /**< Legacy alias. */
    CHAIN_ENV_GET_HUMIDITY_DATA    = CHAIN_ENV_GET_AHT20_DATA, /**< Legacy alias. */
    CHAIN_ENV_GET_PRESSURE_DATA    = CHAIN_ENV_GET_SPA06_DATA, /**< Legacy alias. */
} CHAIN_ENV_CMD_T;                                             /**< Command types for Chain_ENV device operations */

/**
 * @brief Chain ENV enable status enumeration.
 */
typedef enum {
    CHAIN_ENV_DISABLE = 0x00, /**< Disable. */
    CHAIN_ENV_ENABLE  = 0x01, /**< Enable. */
} chain_env_enable_t;

/**
 * @brief Chain ENV SPA06 measurement rate enumeration.
 */
typedef enum {
    CHAIN_ENV_SPA06_RATE_1HZ     = 0x00,
    CHAIN_ENV_SPA06_RATE_2HZ     = 0x01,
    CHAIN_ENV_SPA06_RATE_4HZ     = 0x02,
    CHAIN_ENV_SPA06_RATE_8HZ     = 0x03,
    CHAIN_ENV_SPA06_RATE_16HZ    = 0x04,
    CHAIN_ENV_SPA06_RATE_32HZ    = 0x05,
    CHAIN_ENV_SPA06_RATE_64HZ    = 0x06,
    CHAIN_ENV_SPA06_RATE_128HZ   = 0x07,
    CHAIN_ENV_SPA06_RATE_25_16HZ = 0x08,
    CHAIN_ENV_SPA06_RATE_25_8HZ  = 0x09,
    CHAIN_ENV_SPA06_RATE_25_4HZ  = 0x0A,
    CHAIN_ENV_SPA06_RATE_25_2HZ  = 0x0B,
    CHAIN_ENV_SPA06_RATE_25HZ    = 0x0C,
    CHAIN_ENV_SPA06_RATE_50HZ    = 0x0D,
    CHAIN_ENV_SPA06_RATE_100HZ   = 0x0E,
    CHAIN_ENV_SPA06_RATE_200HZ   = 0x0F,
} chain_env_spa06_rate_t;

/**
 * @brief Chain ENV temperature warning source selection.
 */
typedef enum {
    CHAIN_ENV_TEMPERATURE_SOURCE_AHT20 = 0x00, /**< AHT20 temperature, default. */
    CHAIN_ENV_TEMPERATURE_SOURCE_SPA06 = 0x01, /**< SPA06 temperature. */
} chain_env_temperature_source_t;

/**
 * @brief Enumeration for I2C register address length.
 */
typedef enum {
    CHAIN_ENV_I2C_MEMADD_SIZE_8BIT  = 1, /**< 8-bit address length. */
    CHAIN_ENV_I2C_MEMADD_SIZE_16BIT = 2, /**< 16-bit address length. */
} chain_env_i2c_reg_len_t;

class ChainENV : virtual public ChainCommon {
public:
    /**
     * @brief Reads data from a specific register address of an I2C device through DLight.
     *
     * @param id The position of the DLight device in the chain (starting from 1).
     * @param i2cAddr I2C device address to read data from.
     * @param regAddr Register address to start reading from.
     * @param regLength Length of the register address.
     * @param readLength Number of bytes to read, must not exceed `CHAIN_ENV_I2C_MAX_SIZE` (32 bytes).
     * @param
     * buffer Pointer to the buffer where the read data will be stored.
     * @param operationStatus Pointer to store the operation result (1: success, 0: failure).
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status.
     */
    chain_status_t envI2cMemRead(uint8_t id, uint8_t i2cAddr, uint16_t regAddr, chain_env_i2c_reg_len_t regLength,
                                 uint8_t readLength, uint8_t *buffer, uint8_t *operationStatus,
                                 unsigned long timeout = 100);

    /**
     * @brief Writes data to a specific register address of an I2C device through DLight.
     *
     * @param id The position of the DLight device in the chain (starting from 1).
     * @param i2cAddr I2C device address to write data to.
     * @param regAddr Register address to write data to.
     * @param regLength Length of the register address.
     * @param writeLength Number of bytes to write, must not exceed `CHAIN_ENV_I2C_MAX_SIZE` (32 bytes).
     * For
     * SPA06, do not frequently modify sampling configuration registers through this API.
     * @param buffer Pointer
     * to the buffer containing the data to be written.
     * @param operationStatus Pointer to store the operation result (1: success, 0: failure).
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status.
     */
    chain_status_t envI2cMemWrite(uint8_t id, uint8_t i2cAddr, uint16_t regAddr, chain_env_i2c_reg_len_t regLength,
                                  uint8_t writeLength, uint8_t *buffer, uint8_t *operationStatus,
                                  unsigned long timeout = 100);

    /**
     * @brief Gets AHT20 temperature in Celsius x100 and relative humidity in percent.
     */
    chain_status_t getAHT20Data(uint8_t id, int16_t *temperature, uint8_t *humidity, unsigned long timeout = 100);

    /**
     * @brief Gets SPA06 temperature in Celsius x100, pressure in hPa x10 and altitude in cm.
     */
    chain_status_t getSPA06Data(uint8_t id, int16_t *temperature, uint16_t *pressure, int32_t *altitude,
                                unsigned long timeout = 100);

    /**
     * @brief Sets the SPA06 temperature measurement rate.
     */
    chain_status_t setSPA06TemperatureRate(uint8_t id, chain_env_spa06_rate_t rate, uint8_t *operationStatus,
                                           unsigned long timeout = 100);

    /**
     * @brief Gets the SPA06 temperature measurement rate.
     */
    chain_status_t getSPA06TemperatureRate(uint8_t id, chain_env_spa06_rate_t *rate, unsigned long timeout = 100);

    /**
     * @brief Sets the SPA06 pressure measurement rate.
     */
    chain_status_t setSPA06PressureRate(uint8_t id, chain_env_spa06_rate_t rate, uint8_t *operationStatus,
                                        unsigned long timeout = 100);

    /**
     * @brief Gets the SPA06 pressure measurement rate.
     */
    chain_status_t getSPA06PressureRate(uint8_t id, chain_env_spa06_rate_t *rate, unsigned long timeout = 100);

    /**
     * @brief Sets the SPA06 sea-level pressure used for altitude calculation, in hPa x10.
     */
    chain_status_t setSPA06SeaLevelPressure(uint8_t id, uint16_t pressure, uint8_t *operationStatus,
                                            unsigned long timeout = 100);

    /**
     * @brief Gets the SPA06 sea-level pressure used for altitude calculation, in hPa x10.
     */
    chain_status_t getSPA06SeaLevelPressure(uint8_t id, uint16_t *pressure, unsigned long timeout = 100);

    /**
     * @brief Sets ENV change warning auto-reporting mask (bit0: temp, bit1: humi, bit2: press, bit3: alti).
     */
    chain_status_t setENVChangeEventTriggerMode(uint8_t id, uint8_t mode, uint8_t *operationStatus,
                                                unsigned long timeout = 100);

    /**
     * @brief Gets ENV change warning auto-reporting mask.
     */
    chain_status_t getENVChangeEventTriggerMode(uint8_t id, uint8_t *mode, unsigned long timeout = 100);

    /**
     * @brief Sets the temperature warning source and thresholds in Celsius.
     */
    chain_status_t setENVTemperatureWarningThreshold(uint8_t id, chain_env_temperature_source_t source,
                                                     uint8_t lowThreshold, uint8_t highThreshold,
                                                     uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Gets the temperature warning source and thresholds in Celsius.
     */
    chain_status_t getENVTemperatureWarningThreshold(uint8_t id, chain_env_temperature_source_t *source,
                                                     uint8_t *lowThreshold, uint8_t *highThreshold,
                                                     unsigned long timeout = 100);

    /**
     * @brief Sets the relative humidity warning thresholds in percent.
     */
    chain_status_t setENVHumidityWarningThreshold(uint8_t id, uint8_t lowThreshold, uint8_t highThreshold,
                                                  uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Gets the relative humidity warning thresholds in percent.
     */
    chain_status_t getENVHumidityWarningThreshold(uint8_t id, uint8_t *lowThreshold, uint8_t *highThreshold,
                                                  unsigned long timeout = 100);

    /**
     * @brief Sets the pressure warning thresholds in hPa x10.
     */
    chain_status_t setENVPressureWarningThreshold(uint8_t id, uint16_t lowThreshold, uint16_t highThreshold,
                                                  uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Gets the pressure warning thresholds in hPa x10.
     */
    chain_status_t getENVPressureWarningThreshold(uint8_t id, uint16_t *lowThreshold, uint16_t *highThreshold,
                                                  unsigned long timeout = 100);

    /**
     * @brief Sets the altitude warning thresholds in cm.
     */
    chain_status_t setENVAltitudeWarningThreshold(uint8_t id, int32_t lowThreshold, int32_t highThreshold,
                                                  uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Gets the altitude warning thresholds in cm.
     */
    chain_status_t getENVAltitudeWarningThreshold(uint8_t id, int32_t *lowThreshold, int32_t *highThreshold,
                                                  unsigned long timeout = 100);

    /**
     * @brief Gets the Chain ENV type code.
     */
    uint16_t getENVTypeCode(void);

    /**
     * @brief Gets the latest ENV change warning trigger status.
     *
     * @param triggerStatus Status byte from the 0xE0 auto-report packet.
     *
     * @return true if a trigger record was available.
     */
    bool getENVChangeTrigger(uint8_t id, uint8_t *triggerStatus, unsigned long timeout = 100);

    /**
     * @brief Legacy helper: gets AHT20 temperature in Celsius x100.
     */
    chain_status_t getTemperatureData(uint8_t id, int16_t *temperature, unsigned long timeout = 1000);

    /**
     * @brief Legacy helper: gets AHT20 relative humidity in percent.
     */
    chain_status_t getHumidityData(uint8_t id, uint8_t *humidity, unsigned long timeout = 1000);

    /**
     * @brief Legacy helper: gets SPA06 pressure in hPa x10.
     */
    chain_status_t getPressureData(uint8_t id, uint32_t *pressure, unsigned long timeout = 100);

    /**
     * @brief Legacy helper: gets SPA06 altitude in cm.
     */
    chain_status_t getAltitudeData(uint8_t id, int32_t *altitude, unsigned long timeout = 100);
};

#endif  // CHAIN_ENV_HPP_

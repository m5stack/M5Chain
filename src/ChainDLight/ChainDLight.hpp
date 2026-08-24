/*
 *SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#ifndef CHAIN_DLIGHT_HPP_
#define CHAIN_DLIGHT_HPP_

#include "ChainCommon/ChainCommon.hpp"

/**
 * @brief Maximum size for DLight I2C read/write operations.
 */
#define CHAIN_DLIGHT_I2C_MAX_SIZE (64)

/**
 * @brief Enumeration for CHAIN_DLIGHT device commands.
 */
typedef enum {
    CHAIN_DLIGHT_I2C_MEM_READ                = 0x13, /**< Read data from specific address on I2C device. */
    CHAIN_DLIGHT_I2C_MEM_WRITE               = 0x14, /**< Write data to specific address on I2C device. */
    CHAIN_DLIGHT_GET_LUX                     = 0x30, /**< Get DLight sensor lux value. */
    CHAIN_DLIGHT_SET_LUX_INTERRUPT_ENABLE    = 0x31, /**< Enable/disable lux interrupt measurement. */
    CHAIN_DLIGHT_GET_LUX_INTERRUPT_ENABLE    = 0x32, /**< Get lux interrupt measurement state. */
    CHAIN_DLIGHT_SET_LUX_INTERRUPT_THRESHOLD = 0x33, /**< Set lux interrupt thresholds. */
    CHAIN_DLIGHT_GET_LUX_INTERRUPT_THRESHOLD = 0x34, /**< Get lux interrupt thresholds. */
    CHAIN_DLIGHT_SET_SENSOR_GAIN             = 0x35, /**< Set sensor gain. */
    CHAIN_DLIGHT_GET_SENSOR_GAIN             = 0x36, /**< Get sensor gain. */
    CHAIN_DLIGHT_SET_SENSOR_RESOLUTION       = 0x37, /**< Set sensor resolution. */
    CHAIN_DLIGHT_GET_SENSOR_RESOLUTION       = 0x38, /**< Get sensor resolution. */
    CHAIN_DLIGHT_SET_SENSOR_RATE             = 0x39, /**< Set sensor measurement rate. */
    CHAIN_DLIGHT_GET_SENSOR_RATE             = 0x3A, /**< Get sensor measurement rate. */
    CHAIN_DLIGHT_SET_INTERRUPT_EVENT_TRIGGER = 0xE1, /**< Set interrupt event auto-report mode. */
    CHAIN_DLIGHT_GET_INTERRUPT_EVENT_TRIGGER = 0xE2, /**< Get interrupt event auto-report mode. */
} CHAIN_DLIGHT_CMD_T;

/**
 * @brief Chain DLight enable status enumeration.
 */
typedef enum {
    CHAIN_DLIGHT_DISABLE = 0x00, /**< Disable. */
    CHAIN_DLIGHT_ENABLE  = 0x01, /**< Enable. */
} chain_dlight_enable_t;

typedef enum {
    CHAIN_DLIGHT_REPORT_LUX_LOW   = 0x00, /**< Lux crossed downward through the low threshold. */
    CHAIN_DLIGHT_REPORT_LUX_HIGH  = 0x01, /**< Lux crossed upward through the high threshold. */
    CHAIN_DLIGHT_RELEASE_LUX_LOW  = 0x02, /**< Lux crossed upward through the low threshold. */
    CHAIN_DLIGHT_RELEASE_LUX_HIGH = 0x03, /**< Lux crossed downward through the high threshold. */
} chain_dlight_event_trigger_t;

/**
 * @brief Chain DLight light sensor gain enumeration.
 */
typedef enum {
    CHAIN_DLIGHT_GAIN_1X  = 0x00, /**< Gain x1. */
    CHAIN_DLIGHT_GAIN_3X  = 0x01, /**< Gain x3, default. */
    CHAIN_DLIGHT_GAIN_6X  = 0x02, /**< Gain x6. */
    CHAIN_DLIGHT_GAIN_9X  = 0x03, /**< Gain x9. */
    CHAIN_DLIGHT_GAIN_18X = 0x04, /**< Gain x18. */
} chain_dlight_gain_t;

/**
 * @brief Chain DLight light sensor resolution enumeration.
 */
typedef enum {
    CHAIN_DLIGHT_RESOLUTION_20BIT_400MS = 0x00, /**< 20-bit ALS raw value, 400 ms conversion time. */
    CHAIN_DLIGHT_RESOLUTION_19BIT_200MS = 0x01, /**< 19-bit ALS raw value, 200 ms conversion time. */
    CHAIN_DLIGHT_RESOLUTION_18BIT_100MS = 0x02, /**< 18-bit ALS raw value, 100 ms conversion time, default. */
    CHAIN_DLIGHT_RESOLUTION_17BIT_50MS  = 0x03, /**< 17-bit ALS raw value, 50 ms conversion time. */
    CHAIN_DLIGHT_RESOLUTION_16BIT_25MS  = 0x04, /**< 16-bit ALS raw value, 25 ms conversion time. */
} chain_dlight_resolution_t;

/**
 * @brief Chain DLight light sensor measurement rate enumeration.
 */
typedef enum {
    CHAIN_DLIGHT_RATE_25MS   = 0x00, /**< 25 ms. */
    CHAIN_DLIGHT_RATE_50MS   = 0x01, /**< 50 ms. */
    CHAIN_DLIGHT_RATE_100MS  = 0x02, /**< 100 ms. */
    CHAIN_DLIGHT_RATE_500MS  = 0x03, /**< 500 ms. */
    CHAIN_DLIGHT_RATE_1000MS = 0x04, /**< 1000 ms. */
    CHAIN_DLIGHT_RATE_2000MS = 0x05, /**< 2000 ms. */
} chain_dlight_rate_t;

/**
 * @brief Enumeration for I2C register address length.
 */
typedef enum {
    CHAIN_DLIGHT_I2C_MEMADD_SIZE_8BIT  = 1, /**< 8-bit address length. */
    CHAIN_DLIGHT_I2C_MEMADD_SIZE_16BIT = 2, /**< 16-bit address length. */
} chain_dlight_i2c_reg_len_t;

class ChainDLight : virtual public ChainCommon {
public:
    /**
     * @brief Gets the DLight sensor illuminance value in lux.
     *
     * @param id The position of the DLight device in the chain (starting from 1).
     * @param lux Pointer to store the lux value.
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status.
     */
    chain_status_t getDLightLux(uint8_t id, uint32_t *lux, unsigned long timeout = 100);

    /**
     * @brief Enables or disables lux interrupt measurement.
     *
     * When enabled, lux values are acquired only when they exceed the configured interrupt threshold range.
     *
     * @param id The position of the DLight device in the chain (starting from 1).
     * @param enable Enable status (CHAIN_DLIGHT_ENABLE or CHAIN_DLIGHT_DISABLE).
     * @param operationStatus Pointer to store the operation result (1: success, 0: failure).
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status.
     */
    chain_status_t setDLightLuxInterruptEnable(uint8_t id, chain_dlight_enable_t enable, uint8_t *operationStatus,
                                               unsigned long timeout = 100);

    /**
     * @brief Gets the lux interrupt measurement enable state.
     *
     * @param id The position of the DLight device in the chain (starting from 1).
     * @param enable Pointer to store the enable state.
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status.
     */
    chain_status_t getDLightLuxInterruptEnable(uint8_t id, chain_dlight_enable_t *enable, unsigned long timeout = 100);

    /**
     * @brief Sets the high and low threshold values for lux interrupt measurement.
     *
     * @param id The position of the DLight device in the chain (starting from 1).
     * @param highBoundThreshold High threshold value. Must be greater than lowBoundThreshold and no more than 2516580.
     * @param lowBoundThreshold Low threshold value. Must be no more than 2516580.
     * @param operationStatus Pointer to store the operation result (1: success, 0: failure).
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status.
     */
    chain_status_t setDLightLuxInterruptThreshold(uint8_t id, uint32_t highBoundThreshold, uint32_t lowBoundThreshold,
                                                  uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Gets the high and low threshold values for lux interrupt measurement.
     *
     * @param id The position of the DLight device in the chain (starting from 1).
     * @param highBoundThreshold Pointer to store the high threshold value.
     * @param lowBoundThreshold Pointer to store the low threshold value.
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status.
     */
    chain_status_t getDLightLuxInterruptThreshold(uint8_t id, uint32_t *highBoundThreshold, uint32_t *lowBoundThreshold,
                                                  unsigned long timeout = 100);

    /**
     * @brief Sets the DLight light sensor measurement gain.
     *
     * @param id The position of the DLight device in the chain (starting from 1).
     * @param gain Sensor gain.
     * @param operationStatus Pointer to store the operation result (1: success, 0: failure).
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status.
     */
    chain_status_t setDLightSensorGain(uint8_t id, chain_dlight_gain_t gain, uint8_t *operationStatus,
                                       unsigned long timeout = 100);

    /**
     * @brief Gets the DLight light sensor measurement gain.
     *
     * @param id The position of the DLight device in the chain (starting from 1).
     * @param gain Pointer to store the sensor gain.
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status.
     */
    chain_status_t getDLightSensorGain(uint8_t id, chain_dlight_gain_t *gain, unsigned long timeout = 100);

    /**
     * @brief Sets the DLight light sensor measurement resolution.
     *
     * The configured measurement rate must be
     * strictly greater than the
     * conversion time required by the selected resolution. Equal values are
     *
     * rejected by the device and reported through operationStatus.
     *
     * @param id The position of the DLight
     * device in the chain (starting from 1).
     * @param resolution Sensor resolution.
     * @param operationStatus Pointer to store the operation result (1: success, 0: failure).
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status.
     */
    chain_status_t setDLightSensorResolution(uint8_t id, chain_dlight_resolution_t resolution, uint8_t *operationStatus,
                                             unsigned long timeout = 100);

    /**
     * @brief Gets the DLight light sensor measurement resolution.
     *
     * @param id The position of the DLight device in the chain (starting from 1).
     * @param resolution Pointer to store the sensor resolution.
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status.
     */
    chain_status_t getDLightSensorResolution(uint8_t id, chain_dlight_resolution_t *resolution,
                                             unsigned long timeout = 100);

    /**
     * @brief Sets the DLight light sensor measurement rate.
     *
     * The measurement rate must be strictly greater
     * than the conversion time
     * required by the configured resolution. Equal values are rejected by the
     *
     * device and reported through operationStatus.
     *
     * @param id The position of the DLight device in the chain (starting from 1).
     * @param rate Sensor measurement rate.
     * @param operationStatus Pointer to store the operation result (1: success, 0: failure).
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status.
     */
    chain_status_t setDLightSensorRate(uint8_t id, chain_dlight_rate_t rate, uint8_t *operationStatus,
                                       unsigned long timeout = 100);

    /**
     * @brief Gets the DLight light sensor measurement rate.
     *
     * @param id The position of the DLight device in the chain (starting from 1).
     * @param rate Pointer to store the sensor measurement rate.
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status.
     */
    chain_status_t getDLightSensorRate(uint8_t id, chain_dlight_rate_t *rate, unsigned long timeout = 100);

    /**
     * @brief Reads data from a specific register address of an I2C device through DLight.
     *
     * @param id The position of the DLight device in the chain (starting from 1).
     * @param i2cAddr I2C device address to read data from.
     * @param regAddr Register address to start reading from.
     * @param regLength Length of the register address.
     * @param readLength Number of bytes to read, must not exceed `CHAIN_DLIGHT_I2C_MAX_SIZE`.
     * @param buffer Pointer to the buffer where the read data will be stored.
     * @param operationStatus Pointer to store the operation result (1: success, 0: failure).
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status.
     */
    chain_status_t dlightI2cMemRead(uint8_t id, uint8_t i2cAddr, uint16_t regAddr, chain_dlight_i2c_reg_len_t regLength,
                                    uint8_t readLength, uint8_t *buffer, uint8_t *operationStatus,
                                    unsigned long timeout = 100);

    /**
     * @brief Writes data to a specific register address of an I2C device through DLight.
     *
     * @param id The position of the DLight device in the chain (starting from 1).
     * @param i2cAddr I2C device address to write data to.
     * @param regAddr Register address to write data to.
     * @param regLength Length of the register address.
     * @param writeLength Number of bytes to write, must not exceed `CHAIN_DLIGHT_I2C_MAX_SIZE`.
     * @param buffer Pointer to the buffer containing the data to be written.
     * @param operationStatus Pointer to store the operation result (1: success, 0: failure).
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status.
     */
    chain_status_t dlightI2cMemWrite(uint8_t id, uint8_t i2cAddr, uint16_t regAddr,
                                     chain_dlight_i2c_reg_len_t regLength, uint8_t writeLength, uint8_t *buffer,
                                     uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Enables or disables lux interrupt event auto-reporting.
     *
     * This function is effective only after lux interrupt measurement is enabled.
     *
     * @param id The position of the DLight device in the chain (starting from 1).
     * @param mode Auto-report trigger mode.
     * @param operationStatus Pointer to store the operation result (1: success, 0: failure).
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status.
     */
    chain_status_t setDLightLuxInterruptEventTriggerMode(uint8_t id, chain_dlight_enable_t mode,
                                                         uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Gets the lux interrupt event auto-reporting mode.
     *
     * @param id The position of the DLight device in the chain (starting from 1).
     * @param mode Pointer to store the auto-report trigger mode.
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status.
     */
    chain_status_t getDLightLuxInterruptEventTriggerMode(uint8_t id, chain_dlight_enable_t *mode,
                                                         unsigned long timeout = 100);

    /**
     * @brief Gets the Chain DLight type code.
     *
     * @return Chain DLight device type code.
     */
    uint16_t getDLightTypeCode(void);

    /**
     * @brief Gets the Chain DLight lux threshold event status.
     *
     * @param id The position of the DLight device in the chain (starting from 1).
     * @param triggerStatus Pointer to store the event status. The status uses bit0~bit1 to encode one of four events.

     * *
     * @return true if a trigger status is available, false otherwise.
     */
    bool getDLightLuxTrigger(uint8_t id, chain_dlight_event_trigger_t *triggerStatus);

private:
};

#endif  // CHAIN_DLIGHT_HPP_

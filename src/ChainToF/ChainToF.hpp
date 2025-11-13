/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef _CHAIN_TOF_HPP_
#define _CHAIN_TOF_HPP_

#include "ChainCommon/ChainCommon.hpp"

/**
 * @brief Minimum allowable measurement time for the Chain_ToF sensor (in milliseconds).
 *
 * Shorter integration time results in faster updates but lower accuracy.
 */
#define TOF_MEASUREMENT_TIME_MIN (20)

/**
 * @brief Maximum allowable measurement time for the Chain_ToF sensor (in milliseconds).
 *
 * Longer integration time generally increases accuracy but reduces the update rate.
 */
#define TOF_MEASUREMENT_TIME_MAX (200)

/**
 * @brief Command codes for Chain_ToF sensor operations.
 *
 * These codes are used when communicating with the device over the chain protocol.
 */
typedef enum {
    CHAIN_TOF_GET_DISTANCE              = 0x50, /**< Retrieve distance measurement from the sensor */
    CHAIN_TOF_SET_MEASURE_TIME          = 0x51, /**< Set the measurement (integration) time */
    CHAIN_TOF_GET_MEASURE_TIME          = 0x52, /**< Get the current measurement time */
    CHAIN_TOF_SET_MEASURE_MODE          = 0x53, /**< Set the measurement mode (STOP / SINGLE / CONTINUOUS) */
    CHAIN_TOF_GET_MEASURE_MODE          = 0x54, /**< Get the current measurement mode */
    CHAIN_TOF_SET_MEASURE_STATUS        = 0x55, /**< Start or stop measurement explicitly */
    CHAIN_TOF_GET_MEASURE_STATUS        = 0x56, /**< Get the current measurement status */
    CHAIN_TOF_GET_MEASURE_COMPLETE_FLAG = 0x57  /**< Check if the latest measurement is complete */
} chain_tof_cmd_t;

/**
 * @brief Measure modes for Chain_ToF sensor.
 */
typedef enum {
    CHAIN_TOF_MODE_STOP       = 0, /**< Stop measurements; sensor idle */
    CHAIN_TOF_MODE_SINGLE     = 1, /**< Take a single measurement then stop */
    CHAIN_TOF_MODE_CONTINUOUS = 2  /**< Continue measuring until stopped */
} chain_tof_mode_t;

/**
 * @brief Measure status values for Chain_ToF sensor.
 *
 * Used to explicitly start or stop measurements.
 */
typedef enum {
    CHAIN_TOF_STATUS_STOP  = 0, /**< Stop the measurement process */
    CHAIN_TOF_STATUS_START = 1  /**< Start the measurement process */
} chain_tof_measure_status_t;

/**
 * @class ChainToF
 * @brief Interface for controlling chained Time-of-Flight (ToF) distance sensors.
 *
 * The ChainToF class inherits from ChainCommon and provides high-level methods
 * for querying measurements, setting timing and modes, and controlling the measurement lifecycle.
 * Sensors are addressed by their position in the chain starting from index 1.
 */
class ChainToF : virtual public ChainCommon {
public:
    /**
     * @brief Get the measured distance from the ToF device.
     *
     * @param id Position of the ToF device in the chain (starting from 1).
     * @param distance Pointer to store the measured distance (in mm).
     * @param timeout Timeout duration in milliseconds (default: 100 ms).
     * @return Operation status (e.g., CHAIN_OK, CHAIN_TIMEOUT).
     */
    chain_status_t getToFDistance(uint8_t id, uint16_t *distance, unsigned long timeout = 100);

    /**
     * @brief Set the measurement timing (integration time) for the ToF device.
     *
     * @param id Position of the ToF device in the chain (starting from 1).
     * @param time Measure time in milliseconds.
     *             Must be between TOF_MEASUREMENT_TIME_MIN and TOF_MEASUREMENT_TIME_MAX.
     * @param operationStatus Pointer to store the device's execution result (1 = accepted, 0 = rejected).
     * @param timeout Timeout duration in milliseconds (default: 100 ms).
     * @return Operation status.
     */
    chain_status_t setToFMeasureTime(uint8_t id, uint8_t time, uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Get the current measurement timing from the ToF device.
     *
     * @param id Position of the ToF device in the chain (starting from 1).
     * @param time Pointer to store the measurement time in milliseconds.
     * @param timeout Timeout duration in milliseconds (default: 100 ms).
     * @return Operation status.
     */
    chain_status_t getToFMeasureTime(uint8_t id, uint8_t *time, unsigned long timeout = 100);

    /**
     * @brief Set the measurement mode.
     *
     * @param id Position of the ToF device in the chain (starting from 1).
     * @param mode Desired measurement mode (`chain_tof_mode_t`).
     * @param operationStatus Pointer to store the device's execution result (1 = accepted, 0 = rejected).
     * @param timeout Timeout duration in milliseconds (default: 100 ms).
     * @return Operation status.
     */
    chain_status_t setToFMeasureMode(uint8_t id, chain_tof_mode_t mode, uint8_t *operationStatus,
                                     unsigned long timeout = 100);

    /**
     * @brief Get the current measurement mode from the ToF device.
     *
     * @param id Position of the ToF device in the chain (starting from 1).
     * @param mode Pointer to store the retrieved mode (`chain_tof_mode_t`).
     * @param timeout Timeout duration in milliseconds (default: 100 ms).
     * @return Operation status.
     */
    chain_status_t getToFMeasureMode(uint8_t id, chain_tof_mode_t *mode, unsigned long timeout = 100);

    /**
     * @brief Set the measurement status (START / STOP) for the ToF device.
     *
     * Explicitly starts or stops the measurement process, regardless of the current mode.
     *
     * @param id Position of the ToF device in the chain (starting from 1).
     * @param measureStatus Desired status (`chain_tof_measure_status_t`).
     * @param operationStatus Pointer to store the device's execution result (1 = accepted, 0 = rejected).
     * @param timeout Timeout duration in milliseconds (default: 100 ms).
     * @return Operation status.
     */
    chain_status_t setToFMeasureStatus(uint8_t id, chain_tof_measure_status_t measureStatus, uint8_t *operationStatus,
                                       unsigned long timeout = 100);

    /**
     * @brief Get the current measurement status (START / STOP) from the ToF device.
     *
     * @param id Position of the ToF device in the chain (starting from 1).
     * @param measureStatus Pointer to store the retrieved status (`chain_tof_measure_status_t`).
     * @param timeout Timeout duration in milliseconds (default: 100 ms).
     * @return Operation status.
     */
    chain_status_t getToFMeasureStatus(uint8_t id, chain_tof_measure_status_t *measureStatus,
                                       unsigned long timeout = 100);

    /**
     * @brief Check whether the current measurement cycle is complete.
     *
     * This flag can be used to determine if the latest measurement data is ready for retrieval.
     *
     * @param id Position of the ToF device in the chain (starting from 1).
     * @param completeFlag Pointer to store the completion flag (1 = complete, 0 = not ready).
     * @param timeout Timeout duration in milliseconds (default: 100 ms).
     * @return Operation status.
     */
    chain_status_t getToFMeasureCompleteFlag(uint8_t id, uint8_t *completeFlag, unsigned long timeout = 100);

private:
    // No private members declared, potentially reserved for internal buffer or state tracking.
};

#endif  // _CHAIN_TOF_HPP_
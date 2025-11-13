/*
 *SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#ifndef _CHAIN_ANGLE_HPP_
#define _CHAIN_ANGLE_HPP_

#include "ChainCommon/ChainCommon.hpp"

/**
 * @brief RotationDirection status for Angle device.
 *
 * This enumeration records whether the Angle device's clockwise direction
 * is increasing or decreasing.
 */
typedef enum {
    ANGLE_ROTATION_DECREASING = 0x00, /**< Rotation direction decreasing angle. */
    ANGLE_ROTATION_INCREASING = 0x01, /**< Rotation direction increasing angle. */
} angle_rotation_direction_t;

/**
 * @brief Enumeration for Chain_Angle device commands.
 *
 * This enumeration defines command codes for various operations of the Chain_Angle device.
 */
typedef enum {
    CHAIN_ANGLE_GET_12ADC            = 0x30, /**< Command to get the latest 12-bit ADC value */
    CHAIN_ANGLE_GET_8ADC             = 0x31, /**< Command to get the latest 8-bit mapped ADC value */
    CHAIN_ANGLE_SET_CLOCKWISE_STATUS = 0x32, /**< Command to set the clockwise direction status */
    CHAIN_ANGLE_GET_CLOCKWISE_STATUS = 0x33, /**< Command to get the current clockwise direction status */
} CHAIN_ANGLE_CMD_T;                         /**< Command types for Chain_Angle device operations */

class ChainAngle : virtual public ChainCommon {
public:
    /**
     * @brief Retrieves the 12-bit ADC value of the Angle device.
     *
     * This function fetches the 12-bit ADC value from the specified Angle device.
     *
     * @param id The position of the Angle device in the chain (starting from 1).
     * @param adcValue Pointer to store the retrieved 12-bit ADC value.
     * @param timeout Timeout duration in milliseconds (default is 100ms).
     *
     * @return The operation status (e.g., CHAIN_OK, CHAIN_TIMEOUT, etc.).
     */
    chain_status_t getAngle12BitAdc(uint8_t id, uint16_t *adcValue, unsigned long timeout = 100);

    /**
     * @brief Retrieves the 8-bit ADC value of the Angle device.
     *
     * This function fetches the 8-bit ADC value from the specified Angle device.
     *
     * @param id The position of the Angle device in the chain (starting from 1).
     * @param adcValue Pointer to store the retrieved 8-bit ADC value.
     * @param timeout Timeout duration in milliseconds (default is 100ms).
     *
     * @return The operation status (e.g., CHAIN_OK, CHAIN_TIMEOUT, etc.).
     */
    chain_status_t getAngle8BitAdc(uint8_t id, uint8_t *adcValue, unsigned long timeout = 100);

    /**
     * @brief Sets the rotation direction change status for the angle device at the specified position in the chain.
     *
     * This function sets the rotation direction change status for the angle device. The direction can either be
     * increasing or decreasing in the clockwise direction. By default, the clockwise direction is set to increasing.
     *
     * @param id The position of the angle device in the chain (starting from 1).
     * @param direction The clockwise direction status. Use `ANGLE_ROTATION_INCREASING` for increasing and
     *                  `ANGLE_ROTATION_DECREASING` for decreasing.
     * @param operationStatus Pointer to store the operation status after execution (1 for success, 0 for failure).
     * @param saveToFlash Optional parameter to specify whether to save the setting to flash memory (default is 0, no
     * save).
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return The operation status (e.g., `CHAIN_OK`, `CHAIN_PARAMETER_ERROR`, etc.).
     */
    chain_status_t setAngleRotationDirection(uint8_t id, angle_rotation_direction_t direction, uint8_t *operationStatus,
                                             chain_save_flash_t saveToFlash = CHAIN_SAVE_FLASH_DISABLE,
                                             unsigned long timeout          = 100);

    /**
     * @brief Retrieves the current clockwise rotation change status of the angle device at the specified position in
     * the chain.
     *
     * This function gets the current clockwise rotation change status of the angle device. It determines whether the
     * rotation direction is increasing or decreasing in the clockwise direction.
     *
     * @param id The position of the angle device in the chain (starting from 1).
     * @param direction Pointer to store the current clockwise rotation change status. It will hold
     * `ANGLE_ROTATION_INCREASING` for increasing and `ANGLE_ROTATION_DECREASING` for decreasing.
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return The operation status (e.g., `CHAIN_OK`, `CHAIN_TIMEOUT`, etc.).
     */
    chain_status_t getAngleRotationDirection(uint8_t id, angle_rotation_direction_t *direction,
                                             unsigned long timeout = 100);

private:
};

#endif  // CHAIN_ANGLE_H
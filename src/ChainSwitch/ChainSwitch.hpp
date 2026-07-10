/*
 *SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#ifndef _CHAIN_SWITCH_HPP_
#define _CHAIN_SWITCH_HPP_

#include "ChainCommon/ChainCommon.hpp"

/**
 * @brief Slip direction for Switch device.
 *
 * This enumeration records whether the Switch device's slip
 * direction is increasing or decreasing.
 */
typedef enum {
    CHAIN_SWITCH_DOWNUP_DEC = 0x00, /**< Status indicating the down-to-up direction is decreasing. */
    CHAIN_SWITCH_DOWNUP_INC = 0x01, /**< Status indicating the down-to-up direction is increasing. */
} switch_direction_t;               /**< Enumeration for recording the slip's direction of the switch device. */

typedef enum {
    CHAIN_SWITCH_CLOSE = 0x00, /**< Status indicating the current slip state is closed. */
    CHAIN_SWITCH_OPEN  = 0x01, /**< Status indicating the current slip state is open. */
} switch_status_type_t;        /**< Enumeration for recording the slip's current status of the switch device. */

typedef enum {
    CHAIN_SWITCH_TRIGGER_REPORT_CLOSE = 0x0400, /**< Status indicating the current slip state is closed. */
    CHAIN_SWITCH_TRIGGER_REPORT_OPEN  = 0x0401, /**< Status indicating the current slip state is open. */
} switch_report_type_t; /**< Enumeration for recording the slip's current status of the switch device at Report Mode. */

/**
 * @brief Chain switch slip mode enumeration.
 */
typedef enum {
    CHAIN_SLIP_NONE_REPORT_MODE = 0x00, /**< Disable switch trigger report mode. */
    CHAIN_SLIP_REPORT_MODE      = 0x01, /**< Enable switch trigger report mode. */
} chain_slip_mode_t;

/**
 * @brief Enumeration for CHAIN_SWITCH device commands.
 *
 * This enumeration defines command codes for various operations of the CHAIN_SWITCH device.
 */
typedef enum {
    CHAIN_SWITCH_GET_12ADC                   = 0x30, /**< Command to get the latest 12-bit ADC value */
    CHAIN_SWITCH_GET_8ADC                    = 0x31, /**< Command to get the latest 8-bit mapped ADC value */
    CHAIN_SWITCH_SET_DOWNUP_STATUS           = 0x32, /**< Command to set the slip direction status */
    CHAIN_SWITCH_GET_DOWNUP_STATUS           = 0x33, /**< Command to get the current slip direction status */
    CHAIN_SWITCH_SET_THRESHOLD               = 0x34, /**< Command to set the slip threshold */
    CHAIN_SWITCH_GET_THRESHOLD               = 0x35, /**< Command to get the current slip threshold */
    CHAIN_SWITCH_GET_SWITCH_STATUS           = 0x36, /**< Command to get the current slip status */
    CHAIN_SWITCH_SET_AUTO_SEND_SWITCH_STATUS = 0xE1, /**< Command to set the auto-send switch status */
    CHAIN_SWITCH_GET_AUTO_SEND_SWITCH_STATUS = 0xE2, /**< Command to get the auto-send switch status */
} CHAIN_SWITCH_CMD_T;                                /**< Command types for Chain_Switch device operations */

class ChainSwitch : virtual public ChainCommon {
public:
    /**
     * @brief Retrieves the 12-bit ADC value of the Switch device.
     *
     * This function fetches the 12-bit ADC value from the specified Switch device.
     *
     * @param id The position of the Switch device in the chain (starting from 1).
     * @param adcValue Pointer to store the retrieved 12-bit ADC value.
     * @param timeout Timeout duration in milliseconds (default is 100ms).
     *
     * @return The operation status (e.g., CHAIN_OK, CHAIN_TIMEOUT, etc.).
     */
    chain_status_t getSwitch12BitAdc(uint8_t id, uint16_t *adcValue, unsigned long timeout = 100);

    /**
     * @brief Retrieves the 8-bit ADC value of the Switch device.
     *
     * This function fetches the 8-bit ADC value from the specified Switch device.
     *
     * @param id The position of the Switch device in the chain (starting from 1).
     * @param adcValue Pointer to store the retrieved 8-bit ADC value.
     * @param timeout Timeout duration in milliseconds (default is 100ms).
     *
     * @return The operation status (e.g., CHAIN_OK, CHAIN_TIMEOUT, etc.).
     */
    chain_status_t getSwitch8BitAdc(uint8_t id, uint8_t *adcValue, unsigned long timeout = 100);

    /**
     * @brief Sets the slip direction change status for the Switch device at the specified position in the chain.
     *
     * This function sets the slip direction change status for the Switch device. The direction can either be
     * increasing or decreasing in the down-to-up direction. By default, the down-to-up direction is set to increasing.
     *
     * @param id The position of the Switch device in the chain (starting from 1).
     * @param direction The down-to-up direction status. Use `CHAIN_SWITCH_DOWNUP_INC` for increasing and
     *                  `CHAIN_SWITCH_DOWNUP_DEC` for decreasing.
     * @param operationStatus Pointer to store the operation status after execution (1 for success, 0 for failure).
     * @param saveToFlash Optional parameter to specify whether to save the setting to flash memory (default is 0, no
     * save).
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return The operation status (e.g., `CHAIN_OK`, `CHAIN_PARAMETER_ERROR`, etc.).
     */
    chain_status_t setSwitchDirection(uint8_t id, switch_direction_t direction, uint8_t *operationStatus,
                                      uint8_t saveToFlash = 0, unsigned long timeout = 100);

    /**
     * @brief Retrieves the current slip direction change status of the Switch device at the specified position in
     * the chain.
     *
     * This function gets the current slip direction change status of the Switch device. It determines whether the
     * rotation direction is increasing or decreasing in the down-to-up direction.
     *
     * @param id The position of the Switch device in the chain (starting from 1).
     * @param direction Pointer to store the current slip direction change status. It will hold
     * `CHAIN_SWITCH_DOWNUP_INC` for increasing and `CHAIN_SWITCH_DOWNUP_DEC` for decreasing.
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return The operation status (e.g., `CHAIN_OK`, `CHAIN_TIMEOUT`, etc.).
     */
    chain_status_t getSwitchDirection(uint8_t id, switch_direction_t *direction, unsigned long timeout = 100);

    /**
     * @brief Sets the slip threshold status for the Switch device at the specified position in the chain.
     *
     * This function sets the ADC threshold values used to determine open and close status. The open threshold must be
     * greater than the close threshold, and both values must be 12-bit values from 0 to 4095.
     *
     * @param id The position of the Switch device in the chain (starting from 1).
     * @param open_threshold The slip's open_threshold value. The default value is 3967(0x0F7F).
     * @param close_threshold The slip's close_threshold value. The default value is 80(0x0050).
     * @param operationStatus Pointer to store the operation status after execution (1 for success, 0 for failure).
     * @param saveToFlash Optional parameter to specify whether to save the setting to flash memory (default is 0, no
     * save).
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     * @return The operation status (e.g., `CHAIN_OK`, `CHAIN_PARAMETER_ERROR`, etc.).
     */
    chain_status_t setSwitchThresholdValue(uint8_t id, uint16_t open_threshold, uint16_t close_threshold,
                                           uint8_t *operationStatus, uint8_t saveToFlash = 0,
                                           unsigned long timeout = 100);

    /**
     * @brief Gets the slip threshold values for the Switch device at the specified position in the chain.
     *
     * This function retrieves the ADC threshold values used to determine open and close status.
     *
     * @param id The position of the Switch device in the chain (starting from 1).
     * @param open_threshold Pointer to store the current open_threshold value.
     * @param close_threshold Pointer to store the current close_threshold value.
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     * @return The operation status (e.g., `CHAIN_OK`, `CHAIN_TIMEOUT`, etc.).
     */
    chain_status_t getSwitchThresholdValue(uint8_t id, uint16_t *open_threshold, uint16_t *close_threshold,
                                           unsigned long timeout = 100);

    /**
     * @brief Retrieves the current slip's switch status of the Switch device at the specified position in the chain.
     *
     * This function gets whether the current slip state is open or closed.
     *
     * @param id The position of the Switch device in the chain (starting from 1).
     * @param switch_status Pointer to store the current slip's switch status (`CHAIN_SWITCH_OPEN` or
     * `CHAIN_SWITCH_CLOSE`).
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return The operation status (e.g., `CHAIN_OK`, `CHAIN_TIMEOUT`, etc.).
     */
    chain_status_t getSwitchStatus(uint8_t id, switch_status_type_t *switch_status, unsigned long timeout = 100);

    /**
     * @brief Sets the auto-trigger mode for the Switch device at the specified position in the chain.
     *
     * This function enables or disables switch trigger report mode.
     *
     * @param id The position of the Switch device in the chain (starting from 1).
     * @param auto_status The desired trigger report mode (`CHAIN_SLIP_REPORT_MODE` or
     * `CHAIN_SLIP_NONE_REPORT_MODE`).
     * @param operationStatus Pointer to store the operation status.
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setSwitchAutoTriggerMode(uint8_t id, chain_slip_mode_t auto_status, uint8_t *operationStatus,
                                            unsigned long timeout = 100);

    /**
     * @brief Gets the auto-trigger mode for the Switch device at the specified position in the chain.
     *
     * This function retrieves the current auto-trigger mode of the Switch device.
     *
     * @param id The position of the Switch device in the chain (starting from 1).
     * @param auto_status Pointer to store the current trigger report mode (`CHAIN_SLIP_REPORT_MODE` or
     * `CHAIN_SLIP_NONE_REPORT_MODE`).
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t getSwitchAutoTriggerMode(uint8_t id, chain_slip_mode_t *auto_status, unsigned long timeout = 100);

    /**
     * @brief Gets the triggering report result of the Switch device at the specified position in the chain.
     *
     * @param id The ID of the Switch device in the chain.
     * @param triggerStatus Pointer to store the trigger report status (`CHAIN_SWITCH_TRIGGER_REPORT_OPEN` or
     * `CHAIN_SWITCH_TRIGGER_REPORT_CLOSE`).
     *
     * @return Returns true if a matching trigger report was found, otherwise returns false.
     */
    bool getSwitchTrigger(uint8_t id, switch_report_type_t *triggerStatus);

private:
};

#endif  // _CHAIN_SWITCH_HPP_

/*
 *SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#ifndef _CHAIN_KEY_HPP_
#define _CHAIN_KEY_HPP_

#include <ChainCommon.hpp>

class ChainKey : virtual public ChainCommon {
public:
    /**
     * @brief Get the status of the key button device at a specified chain position.
     *
     * Retrieves whether the key button is currently pressed.
     *
     * @param id The key device position in the chain (starting from 1).
     * @param buttonStatus Pointer to receive the status (1: pressed, 0: not pressed).
     * @param timeout Operation timeout in milliseconds (default 100ms).
     * @return Operation result status code (e.g., CHAIN_OK, CHAIN_PARAMETER_ERROR).
     */
    chain_status_t getKeyButtonStatus(uint8_t id, uint8_t *buttonStatus, unsigned long timeout = 100);

    /**
     * @brief Set the double-click and long-press interval times for the key button device.
     *
     * Configure the required interval durations (in milliseconds) to detect double-click and long-press actions.
     *
     * @param id The position of the key button device in the chain (starting from 1).
     * @param doubleClickIntervalMs Pointer to a variable storing the double-click interval time in milliseconds.
     * @param longPressIntervalMs Pointer to a variable storing the long-press interval time in milliseconds.
     * @param operationStatus Pointer to store the operation result (1 for success, 0 for failure).
     * @param timeout Operation timeout in milliseconds (default is 100 ms).
     *
     * @return Operation status code, e.g., CHAIN_OK for success, CHAIN_PARAMETER_ERROR for invalid parameters.
     */
    chain_status_t setKeyButtonTriggerInterval(uint8_t id, button_double_click_time_t doubleClickIntervalMs,
                                               button_long_press_time_t longPressIntervalMs, uint8_t *operationStatus,
                                               unsigned long timeout = 100);

    /**
     * @brief Get the double-click and long-press interval times for the key button device.
     *
     * Retrieve the current interval durations (in milliseconds) used for detecting double-click and long-press actions.
     *
     * @param id The position of the key button device in the chain (starting from 1).
     * @param doubleClickIntervalMs Pointer to a variable storing the double-click interval time in milliseconds.
     * @param longPressIntervalMs Pointer to a variable storing the long-press interval time in milliseconds.
     * @param timeout Operation timeout in milliseconds (default is 100 ms).
     *
     * @return Operation status code, e.g., CHAIN_OK for success, CHAIN_PARAMETER_ERROR for invalid parameters.
     */
    chain_status_t getKeyButtonTriggerInterval(uint8_t id, button_double_click_time_t *doubleClickIntervalMs,
                                               button_long_press_time_t *longPressIntervalMs,
                                               unsigned long timeout = 100);

    /**
     * @brief Set the operating mode of the key button device.
     *
     * Defines the button behavior mode, such as single-click, double-click, or long-press.
     *
     * @param id The key device position in the chain (starting from 1).
     * @param mode Desired button mode.
     * @param operationStatus Pointer to receive operation result (1: success, 0: failure).
     * @param timeout Operation timeout in milliseconds (default 100ms).
     * @return Operation result status code.
     */
    chain_status_t setKeyButtonMode(uint8_t id, chain_button_mode_t mode, uint8_t *operationStatus,
                                    unsigned long timeout = 100);

    /**
     * @brief Get the current operating mode of the key button device.
     *
     * Retrieves the current button mode setting.
     *
     * @param id The key device position in the chain (starting from 1).
     * @param mode Pointer to receive the current mode.
     * @param timeout Operation timeout in milliseconds (default 100ms).
     * @return Operation result status code.
     */
    chain_status_t getKeyButtonMode(uint8_t id, chain_button_mode_t *mode, unsigned long timeout = 100);

    /**
     * @brief Get the press type status of the key button device.
     *
     * Determines the type of press event (single, double, long).
     *
     * @param id The key device position in the chain (starting from 1).
     * @param keyPressType Pointer to receive the press type.
     * @return true if the key button is currently pressed; false otherwise.
     */
    bool getKeyButtonPressStatus(uint8_t id, chain_button_press_type_t *keyPressType);

private:
};

#endif  // _CHAIN_KEY_HPP_

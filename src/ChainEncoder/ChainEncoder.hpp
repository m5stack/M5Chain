/*
 *SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#ifndef _CHAIN_ENCODER_HPP_
#define _CHAIN_ENCODER_HPP_

#include "ChainCommon/ChainCommon.hpp"

/**
 * @brief Enumeration for AB pin status.
 */
typedef enum {
    ENCODER_AB = 0x00, /**< AB encoding, clockwise increment. */
    ENCODER_BA = 0x01, /**< BA encoding, counterclockwise increment. */
} encoder_ab_t;

/**
 * @brief Command types for the Chain_Encoder.
 */
typedef enum {
    CHAIN_ENCODER_GET_VALUE       = 0x10, /**< Get encoder value. */
    CHAIN_ENCODER_GET_INC_VALUE   = 0x11, /**< Get encoder increment value. */
    CHAIN_ENCODER_RESET_VALUE     = 0x13, /**< Reset encoder value. */
    CHAIN_ENCODER_RESET_INC_VALUE = 0x14, /**< Reset encoder increment value. */
    CHAIN_ENCODER_SET_AB_STATUS   = 0x15, /**< Set AB status, 0->AB, 1->BA. */
    CHAIN_ENCODER_GET_AB_STATUS   = 0x16, /**< Get AB status, 0->AB, 1->BA. */
} chain_encoder_cmd_t;

class ChainEncoder : virtual public ChainCommon {
public:
    /**
     * @brief Retrieves the encoder value.
     *
     * This function retrieves the current encoder value for the specified device in the chain.
     *
     * @param id The position of the device in the chain (starting from 1).
     * @param value Pointer to the variable where the encoder value will be stored.
     * @param timeout The timeout period for the operation (default is 100ms).
     *
     * @return Returns the status of the operation (e.g., success or failure).
     */
    chain_status_t getEncoderValue(uint8_t id, int16_t *value, unsigned long timeout = 100);

    /**
     * @brief Retrieves the encoder increment value.
     *
     * This function retrieves the increment value of the encoder for the specified device in the chain.
     * The increment value represents the change in encoder position since the last read.
     *
     * @param id The position of the device in the chain (starting from 1).
     * @param incValue Pointer to the variable where the increment value will be stored.
     * @param timeout The timeout period for the operation (default is 100ms).
     *
     * @return Returns the status of the operation (e.g., success or failure).
     */
    chain_status_t getEncoderIncValue(uint8_t id, int16_t *incValue, unsigned long timeout = 100);

    /**
     * @brief Resets the encoder value.
     *
     * This function resets the encoder value to its initial state for the specified device in the chain.
     * It is typically used when the encoder value needs to be cleared or initialized.
     *
     * @param id The position of the device in the chain (starting from 1).
     * @param operationStatus Pointer to the variable where the operation status (1 for success, 0 for failure) will be
     * stored.
     * @param timeout The timeout period for the operation (default is 100ms).
     *
     * @return Returns the status of the operation (1 for success, 0 for failure).
     */
    chain_status_t resetEncoderValue(uint8_t id, uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Resets the encoder increment value.
     *
     * This function resets the encoder increment value to its initial state for the specified device in the chain.
     * It is typically used when the increment value needs to be cleared or initialized.
     *
     * @param id The position of the device in the chain (starting from 1).
     * @param operationStatus Pointer to the variable where the operation status (1 for success, 0 for failure) will be
     * stored.
     * @param timeout The timeout period for the operation (default is 100ms).
     *
     * @return Returns the status of the operation (1 for success, 0 for failure).
     */
    chain_status_t resetEncoderIncValue(uint8_t id, uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Sets the encoder AB direction.
     *
     * This function sets the AB direction of the encoder for the specified device in the chain.
     * AB means the direction increases clockwise, and BA means the direction increases counterclockwise.
     *
     * @param id The position of the device in the chain (starting from 1).
     * @param direct The direction to set: 0 for AB (clockwise increase), 1 for BA (counterclockwise increase).
     * @param operationStatus Pointer to the variable where the operation status (1 for success, 0 for failure) will be
     * stored.
     * @param saveToFlash Optional parameter to save the setting to flash memory (default is CHAIN_SAVE_FLASH_DISABLE,
     * not save).
     * @param timeout The timeout period for the operation (default is 100ms).
     *
     * @return Returns the status of the operation (1 for success, 0 for failure).
     */
    chain_status_t setEncoderABDirect(uint8_t id, encoder_ab_t direct, uint8_t *operationStatus,
                                      chain_save_flash_t saveToFlash = CHAIN_SAVE_FLASH_DISABLE,
                                      unsigned long timeout          = 100);

    /**
     * @brief Gets the encoder AB direction.
     *
     * This function retrieves the current AB direction of the encoder for the specified device in the chain.
     * AB means the direction increases clockwise, and BA means the direction increases counterclockwise.
     *
     * @param id The position of the device in the chain (starting from 1).
     * @param direct Pointer to the variable where the direction (0 for AB, 1 for BA) will be stored.
     * @param timeout The timeout period for the operation (default is 100ms).
     *
     * @return Returns the status of the operation (1 for success, 0 for failure).
     */
    chain_status_t getEncoderABDirect(uint8_t id, encoder_ab_t *direct, unsigned long timeout = 100);

    /**
     * @brief Get the status of the encoder button device at a specified chain position.
     *
     * Retrieves whether the encoder button is currently pressed.
     *
     * @param id The encoder button device position in the chain (starting from 1).
     * @param buttonStatus Pointer to receive the status (1: pressed, 0: not pressed).
     * @param timeout Operation timeout in milliseconds (default 100ms).
     * @return Operation result status code (e.g., CHAIN_OK, CHAIN_PARAMETER_ERROR).
     */
    chain_status_t getEncoderButtonStatus(uint8_t id, uint8_t *buttonStatus, unsigned long timeout = 100);

    /**
     * @brief Set the double-click and long-press interval times for the encoder button device.
     *
     * Configure the required interval durations (in milliseconds) to detect double-click and long-press actions.
     *
     * @param id The position of the encoder button device in the chain (starting from 1).
     * @param doubleClickIntervalMs Pointer to a variable storing the double-click interval time in milliseconds.
     * @param longPressIntervalMs Pointer to a variable storing the long-press interval time in milliseconds.
     * @param operationStatus Pointer to store the operation result (1 for success, 0 for failure).
     * @param timeout Operation timeout in milliseconds (default is 100 ms).
     *
     * @return Operation status code, e.g., CHAIN_OK for success, CHAIN_PARAMETER_ERROR for invalid parameters.
     */
    chain_status_t setEncoderButtonTriggerInterval(uint8_t id, button_double_click_time_t doubleClickIntervalMs,
                                                   button_long_press_time_t longPressIntervalMs,
                                                   uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Get the double-click and long-press interval times for the encoder button device.
     *
     * Retrieve the current interval durations (in milliseconds) used for detecting double-click and long-press actions.
     *
     * @param id The position of the encoder button device in the chain (starting from 1).
     * @param doubleClickIntervalMs Pointer to a variable storing the double-click interval time in milliseconds.
     * @param longPressIntervalMs Pointer to a variable storing the long-press interval time in milliseconds.
     * @param timeout Operation timeout in milliseconds (default is 100 ms).
     *
     * @return Operation status code, e.g., CHAIN_OK for success, CHAIN_PARAMETER_ERROR for invalid parameters.
     */
    chain_status_t getEncoderButtonTriggerInterval(uint8_t id, button_double_click_time_t *doubleClickIntervalMs,
                                                   button_long_press_time_t *longPressIntervalMs,
                                                   unsigned long timeout = 100);

    /**
     * @brief Set the operating mode of the encoder button device.
     *
     * Defines the button behavior mode, such as single-click, double-click, or long-press.
     *
     * @param id The encoder device position in the chain (starting from 1).
     * @param mode Desired encoder button mode.
     * @param operationStatus Pointer to receive operation result (1: success, 0: failure).
     * @param timeout Operation timeout in milliseconds (default 100ms).
     * @return Operation result status code.
     */
    chain_status_t setEncoderButtonMode(uint8_t id, chain_button_mode_t mode, uint8_t *operationStatus,
                                        unsigned long timeout = 100);

    /**
     * @brief Get the current operating mode of the encoder button device.
     *
     * Retrieves the current button mode setting.
     *
     * @param id The encoder device position in the chain (starting from 1).
     * @param mode Pointer to receive the current mode.
     * @param timeout Operation timeout in milliseconds (default 100ms).
     * @return Operation result status code.
     */
    chain_status_t getEncoderButtonMode(uint8_t id, chain_button_mode_t *mode, unsigned long timeout = 100);

    /**
     * @brief Get the press type status of the encoder button device.
     *
     * Determines the type of press event (single, double, long).
     *
     * @param id The encoder device position in the chain (starting from 1).
     * @param encoderPressType Pointer to receive the press type.
     * @return true if the encoder button is currently pressed; false otherwise.
     */
    bool getEncoderButtonPressStatus(uint8_t id, chain_button_press_type_t *encoderPressType);

private:
};

#endif  // _CHAIN_ENCODER_HPP_

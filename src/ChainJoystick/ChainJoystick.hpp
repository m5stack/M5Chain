/*
 *SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#ifndef _CHAIN_JOYSTICK_HPP_
#define _CHAIN_JOYSTICK_HPP_

#include <ChainCommon.hpp>

/**
 * @brief Size of the joystick map buffer.
 */
#define JOYSTICK_MAP_SIZE (8)

/**
 * @brief Enumeration for Chain_Joystick commands.
 */
typedef enum {
    CHAIN_JOYSTICK_GET_16ADC                     = 0x30, /**< Command to get 16-bit ADC values */
    CHAIN_JOYSTICK_GET_8ADC                      = 0x31, /**< Command to get 8-bit ADC values */
    CHAIN_JOYSTICK_GET_ADC_XY_MAPPED_RANGE       = 0x32, /**< Command to get mapped range for X and Y axes */
    CHAIN_JOYSTICK_SET_ADC_XY_MAPPED_RANGE       = 0x33, /**< Command to set mapped range for X and Y axes */
    CHAIN_JOYSTICK_GET_ADC_XY_MAPPED_INT16_VALUE = 0x34, /**< Command to get 16-bit mapped values for X and Y */
    CHAIN_JOYSTICK_GET_ADC_XY_MAPPED_INT8_VALUE  = 0x35, /**< Command to get 8-bit mapped values for X and Y */
} chain_joystick_cmd_t;

class ChainJoystick : virtual public ChainCommon {
public:
    /**
     * @brief Retrieves the ADC values of the joystick.
     *
     * This function reads the ADC values of the joystick for the device at the specified position in the chain.
     * It returns the ADC values for both the X and Y axes.
     *
     * @param id The position of the device in the chain (starting from 1).
     * @param xAdcValue Pointer to store the ADC value for the X-axis.
     * @param yAdcValue Pointer to store the ADC value for the Y-axis.
     * @param timeout Maximum time to wait for the ADC values in milliseconds (default: 100ms).
     *
     * @return The operation status (e.g., CHAIN_OK, CHAIN_TIMEOUT, etc.).
     */
    chain_status_t getJoystick16Adc(uint8_t id, uint16_t *xAdcValue, uint16_t *yAdcValue, unsigned long timeout = 100);

    /**
     * @brief Retrieves the 8-bit ADC values of the joystick.
     *
     * This function reads the 8-bit ADC values of the joystick for the device at the specified position in the chain.
     * It returns the 8-bit ADC values for both the X and Y axes.
     *
     * @param id The position of the device in the chain (starting from 1).
     * @param xAdcValue Pointer to store the 8-bit ADC value for the X-axis.
     * @param yAdcValue Pointer to store the 8-bit ADC value for the Y-axis.
     * @param timeout Maximum time to wait for the ADC values in milliseconds (default: 100ms).
     *
     * @return The operation status (e.g., CHAIN_OK, CHAIN_TIMEOUT, etc.).
     */
    chain_status_t getJoystick8Adc(uint8_t id, uint8_t *xAdcValue, uint8_t *yAdcValue, unsigned long timeout = 100);

    /**
     * @brief Retrieves the joystick mapped range values.
     *
     * This function retrieves the mapped range values of the joystick at the specified position,
     * typically mapping the joystick's X and Y ADC values to a specified range.
     * The mapped values are stored in the provided `mapBufValue` buffer.
     *
     * @param id The position of the device in the chain (starting from 1).
     * @param mapBufValue The buffer where the retrieved mapped range values will be stored.
     * @param size The size of the buffer, limiting the number of mapped values to be returned.
     * @param timeout The timeout duration for retrieving the data in milliseconds (default: 100ms).
     *
     * @return The operation status (e.g., `CHAIN_OK`, `CHAIN_TIMEOUT`, etc.).
     */
    chain_status_t getJoystickMappedRange(uint8_t id, uint16_t *mapBufValue, uint8_t size, unsigned long timeout = 100);

    /**
     * @brief Sets the joystick mapped range values.
     *
     * This function sets the mapped range values for the joystick at the specified position.
     * The mapped values are set based on the data in the provided `mapBufValue` buffer.
     * Optionally, the settings can be saved to flash memory.
     *
     * @param id The position of the device in the chain (starting from 1).
     * @param mapBufValue The buffer containing the mapped range values to be set.
     * @param size The size of the buffer, indicating the number of mapped values.
     * @param operationStatus A pointer to store the operation status after completion. Returns 1 if successful, 0 if
     * failed.
     * @param saveToFlash Whether to save the settings to flash memory; 0 for no, 1 for yes (default: 0).
     * @param timeout The timeout duration for the operation in milliseconds (default: 100ms).
     *
     * @return The operation status (e.g., `CHAIN_OK`, `CHAIN_TIMEOUT`, etc.).
     */
    chain_status_t setJoystickMappedRange(uint8_t id, uint16_t *mapBufValue, uint8_t size, uint8_t *operationStatus,
                                          chain_save_flash_t saveToFlash = CHAIN_SAVE_FLASH_DISABLE,
                                          unsigned long timeout          = 100);
    /**
     * @brief Maps the joystick's 16-bit ADC values to final signed int16 range values.
     *
     * This function maps the joystick's X and Y axis 16-bit ADC values to signed int16 range values based on the
     * joystick's internal mapping range. The mapping process converts the ADC values to the appropriate range according
     * to preset intervals, returning the final mapped X and Y values. The returned values are signed int16,
     * representing the mapped results of the joystick's X and Y axes.
     *
     * @param id The position of the device in the chain (starting from 1).
     * @param xMapAdcValue A pointer to store the mapped value for the joystick's X-axis (int16 type).
     * @param yMapAdcValue A pointer to store the mapped value for the joystick's Y-axis (int16 type).
     * @param timeout The timeout duration for the operation in milliseconds (default: 100ms).
     *
     * @return The operation status (e.g., `CHAIN_OK`, `CHAIN_TIMEOUT`, etc.).
     */
    chain_status_t getJoystickMappedInt16Value(uint8_t id, int16_t *xMapAdcValue, int16_t *yMapAdcValue,
                                               unsigned long timeout = 100);
    /**
     * @brief Maps the joystick's 16-bit ADC values to final signed int8 range values.
     *
     * This function maps the joystick's X and Y axis 16-bit ADC values to signed int8 range values based on the
     * joystick's internal mapping range. The mapping process converts the ADC values to the appropriate range according
     * to preset intervals, returning the final mapped X and Y values. The returned values are signed int8, representing
     * the mapped results of the joystick's X and Y axes with a range of -127 to 127.
     *
     * @param id The position of the device in the chain (starting from 1).
     * @param xMapAdcValue A pointer to store the mapped value for the joystick's X-axis (int8 type).
     * @param yMapAdcValue A pointer to store the mapped value for the joystick's Y-axis (int8 type).
     * @param timeout The timeout duration for the operation in milliseconds (default: 100ms).
     *
     * @return The operation status (e.g., `CHAIN_OK`, `CHAIN_TIMEOUT`, etc.).
     */
    chain_status_t getJoystickMappedInt8Value(uint8_t id, int8_t *xMapAdcValue, int8_t *yMapAdcValue,
                                              unsigned long timeout = 100);

    /**
     * @brief Get the status of the joystick button device at a specified chain position.
     *
     * Retrieves whether the joystick button is currently pressed.
     *
     * @param id The joystick button device position in the chain (starting from 1).
     * @param buttonStatus Pointer to receive the status (1: pressed, 0: not pressed).
     * @param timeout Operation timeout in milliseconds (default 100ms).
     * @return Operation result status code (e.g., CHAIN_OK, CHAIN_PARAMETER_ERROR).
     */
    chain_status_t getJoystickButtonStatus(uint8_t id, uint8_t *buttonStatus, unsigned long timeout = 100);

    /**
     * @brief Set the double-click and long-press interval times for the joystick button device.
     *
     * Configure the required interval durations (in milliseconds) to detect double-click and long-press actions.
     *
     * @param id The position of the joystick button device in the chain (starting from 1).
     * @param doubleClickIntervalMs Pointer to a variable storing the double-click interval time in milliseconds.
     * @param longPressIntervalMs Pointer to a variable storing the long-press interval time in milliseconds.
     * @param operationStatus Pointer to store the operation result (1 for success, 0 for failure).
     * @param timeout Operation timeout in milliseconds (default is 100 ms).
     *
     * @return Operation status code, e.g., CHAIN_OK for success, CHAIN_PARAMETER_ERROR for invalid parameters.
     */
    chain_status_t setJoystickButtonTriggerInterval(uint8_t id, button_double_click_time_t doubleClickIntervalMs,
                                               button_long_press_time_t longPressIntervalMs, uint8_t *operationStatus,
                                               unsigned long timeout = 100);

    /**
     * @brief Get the double-click and long-press interval times for the joystick button device.
     *
     * Retrieve the current interval durations (in milliseconds) used for detecting double-click and long-press actions.
     *
     * @param id The position of the joystick button device in the chain (starting from 1).
     * @param doubleClickIntervalMs Pointer to a variable storing the double-click interval time in milliseconds.
     * @param longPressIntervalMs Pointer to a variable storing the long-press interval time in milliseconds.
     * @param timeout Operation timeout in milliseconds (default is 100 ms).
     *
     * @return Operation status code, e.g., CHAIN_OK for success, CHAIN_PARAMETER_ERROR for invalid parameters.
     */
    chain_status_t getJoystickButtonTriggerInterval(uint8_t id, button_double_click_time_t *doubleClickIntervalMs,
                                               button_long_press_time_t *longPressIntervalMs,
                                               unsigned long timeout = 100);

    /**
     * @brief Set the operating mode of the joystick button device.
     *
     * Defines the button behavior mode, such as single-click, double-click, or long-press.
     *
     * @param id The joystick button device position in the chain (starting from 1).
     * @param mode Desired button mode.
     * @param operationStatus Pointer to receive operation result (1: success, 0: failure).
     * @param timeout Operation timeout in milliseconds (default 100ms).
     * @return Operation result status code.
     */
    chain_status_t setJoystickButtonMode(uint8_t id, chain_button_mode_t mode, uint8_t *operationStatus,
                                         unsigned long timeout = 100);

    /**
     * @brief Get the current operating mode of the joystick button device.
     *
     * Retrieves the current button mode setting.
     *
     * @param id The joystick button device position in the chain (starting from 1).
     * @param mode Pointer to receive the current mode.
     * @param timeout Operation timeout in milliseconds (default 100ms).
     * @return Operation result status code.
     */
    chain_status_t getJoystickButtonMode(uint8_t id, chain_button_mode_t *mode, unsigned long timeout = 100);

    /**
     * @brief Get the press type status of the joystick button device.
     *
     * Determines the type of press event (single, double, long).
     *
     * @param id The joystick button device position in the chain (starting from 1).
     * @param joystickPressType Pointer to receive the press type.
     * @return true if the joystick button is currently pressed; false otherwise.
     */
    bool getJoystickButtonPressStatus(uint8_t id, chain_button_press_type_t *joystickPressType);

private:
};

#endif  // _CHAIN_JOYSTICK_HPP_

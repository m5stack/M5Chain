/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef _UNIT_8SERVOS2_CHAIN_HPP_
#define _UNIT_8SERVOS2_CHAIN_HPP_

#include "ChainCommon/ChainCommon.hpp"

#define SERVOS2_GPIO_NUM_MAX (8) /**< Maximum number of GPIO pins */

#define RGB_NUM_MAX (16) /**< Maximum number of RGB LEDs */

/**
 * @brief Command codes for Servos2 chain operations.
 */
typedef enum {
    SERVOS2_CHAIN_SET_MODE              = 0x10, /**< Set single GPIO mode */
    SERVOS2_CHAIN_SET_MODE_ALL          = 0x11, /**< Set all GPIO modes */
    SERVOS2_CHAIN_GET_MODE              = 0x12, /**< Get single GPIO mode */
    SERVOS2_CHAIN_GET_MODE_ALL          = 0x13, /**< Get all GPIO modes */
    SERVOS2_CHAIN_SET_INPUT_PU_PD       = 0x30, /**< Set single input pull-up/down configuration */
    SERVOS2_CHAIN_SET_INPUT_PU_PD_ALL   = 0x31, /**< Set all input pull-up/down configurations */
    SERVOS2_CHAIN_GET_INPUT_PU_PD       = 0x32, /**< Get single input pull-up/down configuration */
    SERVOS2_CHAIN_GET_INPUT_PU_PD_ALL   = 0x33, /**< Get all input pull-up/down configurations */
    SERVOS2_CHAIN_GET_INPUT_STATUS      = 0x34, /**< Get single input digital level */
    SERVOS2_CHAIN_GET_INPUT_STATUS_ALL  = 0x35, /**< Get all input digital levels */
    SERVOS2_CHAIN_SET_OUTPUT_STATUS     = 0x40, /**< Set single output digital level */
    SERVOS2_CHAIN_SET_OUTPUT_STATUS_ALL = 0x41, /**< Set all output digital levels */
    SERVOS2_CHAIN_GET_OUTPUT_STATUS     = 0x42, /**< Get single output digital level */
    SERVOS2_CHAIN_GET_OUTPUT_STATUS_ALL = 0x43, /**< Get all output digital levels */
    SERVOS2_CHAIN_GET_ADC_VALUE         = 0x50, /**< Get single ADC value */
    SERVOS2_CHAIN_GET_ADC_VALUE_ALL     = 0x51, /**< Get all ADC values */
    SERVOS2_CHAIN_SET_SERVO_ANGLE       = 0x60, /**< Set single servo angle */
    SERVOS2_CHAIN_SET_SERVO_ANGLE_ALL   = 0x61, /**< Set all servo angles */
    SERVOS2_CHAIN_GET_SERVO_ANGLE       = 0x62, /**< Get single servo angle */
    SERVOS2_CHAIN_GET_SERVO_ANGLE_ALL   = 0x63, /**< Get all servo angles */
    SERVOS2_CHAIN_SET_RGB_CONFIG        = 0x70, /**< Set single RGB configuration */
    SERVOS2_CHAIN_SET_RGB_CONFIG_ALL    = 0x71, /**< Set all RGB configurations */
    SERVOS2_CHAIN_GET_RGB_CONFIG        = 0x72, /**< Get single RGB configuration */
    SERVOS2_CHAIN_GET_RGB_CONFIG_ALL    = 0x73, /**< Get all RGB configurations */
    SERVOS2_CHAIN_SET_RGB_BUFFER        = 0x74, /**< Set single RGB color buffer */
    SERVOS2_CHAIN_SET_RGB_BUFFER_ALL    = 0x75, /**< Set all RGB color buffers */
    SERVOS2_CHAIN_GET_RGB_BUFFER        = 0x76, /**< Get single RGB color buffer */
    SERVOS2_CHAIN_GET_RGB_BUFFER_ALL    = 0x77, /**< Get all RGB color buffers */
    SERVOS2_CHAIN_SET_PWM_DUTY          = 0x80, /**< Set single PWM duty cycle */
    SERVOS2_CHAIN_SET_PWM_DUTY_ALL      = 0x81, /**< Set all PWM duty cycles */
    SERVOS2_CHAIN_GET_PWM_DUTY          = 0x82, /**< Get single PWM duty cycle */
    SERVOS2_CHAIN_GET_PWM_DUTY_ALL      = 0x83, /**< Get all PWM duty cycles */
    SERVOS2_CHAIN_SET_TIME_CONFIG       = 0x90, /**< Set single timer frequency configuration */
    SERVOS2_CHAIN_SET_TIME_CONFIG_ALL   = 0x91, /**< Set all timer frequency configurations */
    SERVOS2_CHAIN_GET_TIME_CONFIG       = 0x92, /**< Get single timer frequency configuration */
    SERVOS2_CHAIN_GET_TIME_CONFIG_ALL   = 0x93, /**< Get all timer frequency configurations */
    SERVOS2_CHAIN_GET_VREF              = 0xA0, /**< Get system reference voltage */
    SERVOS2_CHAIN_GET_GROVE_VOLTAGE     = 0xA1, /**< Get Grove interface voltage */
    SERVOS2_CHAIN_GET_DC_VOLTAGE        = 0xA2, /**< Get DC input voltage */
    SERVOS2_CHAIN_GET_SYS_CURRENT       = 0xA3, /**< Get system current consumption */
} servos2_chain_cmd_t;

/**
 * @brief Enumeration of supported GPIO operation modes.
 */
typedef enum {
    USER_GPIO_INPUT_MODE  = 0x00, /**< Digital input mode */
    USER_GPIO_OUTPUT_MODE = 0x01, /**< Digital output mode */
    USER_GPIO_ADC_MODE    = 0x02, /**< Analog-to-digital converter mode */
    USER_GPIO_SERVO_MODE  = 0x03, /**< Servo control mode */
    USER_GPIO_RGB_MODE    = 0x04, /**< RGB LED control mode */
    USER_GPIO_PWM_MODE    = 0x05, /**< PWM output mode */
} user_gpio_mode_t;

/**
 * @brief Enumeration of GPIO pull resistor configurations.
 */
typedef enum {
    USER_GPIO_PULL_NONE = 0x00, /**< No internal pull resistor */
    USER_GPIO_PULL_UP   = 0x01, /**< Internal pull-up resistor enabled */
    USER_GPIO_PULL_DOWN = 0x02, /**< Internal pull-down resistor enabled */
} user_gpio_pull_t;

/**
 * @brief Enumeration of GPIO logic levels.
 */
typedef enum {
    USER_GPIO_LEVEL_LOW  = 0x00, /**< Low level (Logic 0) */
    USER_GPIO_LEVEL_HIGH = 0x01, /**< High level (Logic 1) */
} user_sys_gpio_level_t;

class Unit8Servos2Chain : virtual public ChainCommon {
public:
    /**
     * @brief Sets the operation mode for a single GPIO channel.
     *
     * Configures the specified GPIO channel to one of the supported modes (Input, Output, ADC, Servo, RGB, PWM).
     * The operation status is returned via the `operationStatus` parameter.
     *
     * @param id Device position in the chain (starting from 1).
     * @param gpio The GPIO channel to configure (e.g., `GPIO_CHANNEL_0`).
     * @param mode The desired operation mode (refer to `user_gpio_mode_t`).
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation (success or failure).
     */
    chain_status_t setServosMode(uint8_t id, uint8_t gpio, user_gpio_mode_t mode, uint8_t *operationStatus,
                                 unsigned long timeout = 100);

    /**
     * @brief Sets the operation mode for all GPIO channels.
     *
     * Batch configures the operation modes for all supported GPIO channels.
     * The `mode` parameter should point to an array containing the modes for each channel.
     *
     * @param id Device position in the chain (starting from 1).
     * @param mode Pointer to an array of `user_gpio_mode_t` specifying the mode for each channel.
     * @param num The number of modes/channels to set.
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t setServosModeAll(uint8_t id, user_gpio_mode_t *mode, uint8_t num, uint8_t *operationStatus,
                                    unsigned long timeout = 100);

    /**
     * @brief Gets the operation mode of a single GPIO channel.
     *
     * Retrieves the current configuration mode of the specified GPIO channel.
     *
     * @param id Device position in the chain (starting from 1).
     * @param gpio The GPIO channel to query (e.g., `GPIO_CHANNEL_0`).
     * @param mode Pointer to store the retrieved operation mode (refer to `user_gpio_mode_t`).
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosMode(uint8_t id, uint8_t gpio, user_gpio_mode_t *mode, uint8_t *operationStatus,
                                 unsigned long timeout = 100);

    /**
     * @brief Gets the operation mode of all GPIO channels.
     *
     * Retrieves the current configuration modes for all GPIO channels simultaneously.
     *
     * @param id Device position in the chain (starting from 1).
     * @param mode Pointer to a buffer where the operation modes will be stored.
     * @param num The expected number of channels to read.
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosModeAll(uint8_t id, user_gpio_mode_t *mode, uint8_t num, unsigned long timeout = 100);

    /**
     * @brief Sets the input pull-up/pull-down configuration for a single GPIO.
     *
     * Configures the internal pull-up or pull-down resistor for a GPIO channel when in input mode.
     *
     * @param id Device position in the chain (starting from 1).
     * @param gpio The GPIO channel to configure.
     * @param pu_pd The pull configuration to set (refer to `user_gpio_pull_t`).
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t setServosInputPuPd(uint8_t id, uint8_t gpio, user_gpio_pull_t pu_pd, uint8_t *operationStatus,
                                      unsigned long timeout = 100);

    /**
     * @brief Sets the input pull-up/pull-down configuration for all GPIOs.
     *
     * Batch configures the internal pull resistors for all GPIO channels.
     *
     * @param id Device position in the chain (starting from 1).
     * @param pu_pd Pointer to an array containing the pull configuration for each channel.
     * @param size The number of channels to configure.
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t setServosInputPuPdAll(uint8_t id, user_gpio_pull_t *pu_pd, uint8_t size, uint8_t *operationStatus,
                                         unsigned long timeout = 100);

    /**
     * @brief Gets the input pull-up/pull-down configuration of a single GPIO.
     *
     * Retrieves the current pull resistor configuration of the specified GPIO channel.
     *
     * @param id Device position in the chain (starting from 1).
     * @param gpio The GPIO channel to query.
     * @param pu_pd Pointer to store the retrieved pull configuration.
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosInputPuPd(uint8_t id, uint8_t gpio, user_gpio_pull_t *pu_pd, uint8_t *operationStatus,
                                      unsigned long timeout = 100);

    /**
     * @brief Gets the input pull-up/pull-down configuration of all GPIOs.
     *
     * Retrieves the current pull resistor configurations for all GPIO channels.
     *
     * @param id Device position in the chain (starting from 1).
     * @param pu_pd Pointer to a buffer where the pull configurations will be stored.
     * @param size The number of channels to read.
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosInputPuPdAll(uint8_t id, user_gpio_pull_t *pu_pd, uint8_t size,
                                         unsigned long timeout = 100);

    /**
     * @brief Reads the input status (level) of a single GPIO.
     *
     * Reads the digital input level (high/low) of the specified GPIO channel.
     *
     * @param id Device position in the chain (starting from 1).
     * @param gpio The GPIO channel to read.
     * @param level Pointer to store the input level (refer to `user_sys_gpio_level_t`).
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosInputLevel(uint8_t id, uint8_t gpio, user_sys_gpio_level_t *level, uint8_t *operationStatus,
                                       unsigned long timeout = 100);

    /**
     * @brief Reads the input status (level) of all GPIOs.
     *
     * Reads the digital input levels for all GPIO channels.
     *
     * @param id Device position in the chain (starting from 1).
     * @param level Pointer to a buffer where the input levels will be stored.
     * @param size The number of channels to read.
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosInputLevelAll(uint8_t id, user_sys_gpio_level_t *level, uint8_t size,
                                          unsigned long timeout = 100);

    /**
     * @brief Sets the output status (level) of a single GPIO.
     *
     * Sets the digital output level (high/low) for the specified GPIO channel.
     * The channel must be configured in output mode.
     *
     * @param id Device position in the chain (starting from 1).
     * @param gpio The GPIO channel to control.
     * @param level The logic level to set (refer to `user_sys_gpio_level_t`).
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t setServosOutputLevel(uint8_t id, uint8_t gpio, user_sys_gpio_level_t level, uint8_t *operationStatus,
                                        unsigned long timeout = 100);

    /**
     * @brief Sets the output status (level) of all GPIOs.
     *
     * Batch sets the digital output levels for all GPIO channels.
     *
     * @param id Device position in the chain (starting from 1).
     * @param level Pointer to an array containing the output levels for each channel.
     * @param size The number of channels to set.
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t setServosOutputLevelAll(uint8_t id, user_sys_gpio_level_t *level, uint8_t size,
                                           uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Gets the output status (level) of a single GPIO.
     *
     * Reads the current output level setting of the specified GPIO channel.
     *
     * @param id Device position in the chain (starting from 1).
     * @param gpio The GPIO channel to query.
     * @param level Pointer to store the retrieved current output level.
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosOutputLevel(uint8_t id, uint8_t gpio, user_sys_gpio_level_t *level,
                                        uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Gets the output status (level) of all GPIOs.
     *
     * Reads the current output levels of all GPIO channels.
     *
     * @param id Device position in the chain (starting from 1).
     * @param level Pointer to a buffer where the output levels will be stored.
     * @param size The number of channels to read.
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosOutputLevelAll(uint8_t id, user_sys_gpio_level_t *level, uint8_t size,
                                           unsigned long timeout = 100);

    /**
     * @brief Reads the ADC value from a single GPIO.
     *
     * Reads the analog value from the specified GPIO channel. The channel must be in ADC mode.
     *
     * @param id Device position in the chain (starting from 1).
     * @param gpio The GPIO channel to read.
     * @param value Pointer to store the read ADC value.
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosAdcValue(uint8_t id, uint8_t gpio, uint16_t *value, uint8_t *operationStatus,
                                     unsigned long timeout = 100);

    /**
     * @brief Reads the ADC values from all GPIOs.
     *
     * Reads the analog values from all GPIO channels simultaneously.
     *
     * @param id Device position in the chain (starting from 1).
     * @param value Pointer to a buffer where the ADC values will be stored.
     * @param size The number of channels to read.
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosAdcValueAll(uint8_t id, uint16_t *value, uint8_t size, unsigned long timeout = 100);

    /**
     * @brief Sets the servo angle for a single GPIO.
     *
     * Controls the servo connected to the specified GPIO channel by setting its target angle.
     * The channel must be configured in Servo mode.
     *
     * @param id Device position in the chain (starting from 1).
     * @param gpio The GPIO channel to control.
     * @param angle The angle to set (typically 0-180 degrees).
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t setServosAngle(uint8_t id, uint8_t gpio, uint8_t angle, uint8_t *operationStatus,
                                  unsigned long timeout = 100);

    /**
     * @brief Sets the servo angles for all GPIOs.
     *
     * Batch controls the servos connected to all GPIO channels by setting their angles.
     *
     * @param id Device position in the chain (starting from 1).
     * @param angle Pointer to an array containing the angles for each servo.
     * @param size The number of channels/servos to set.
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t setServosAngleAll(uint8_t id, uint8_t *angle, uint8_t size, uint8_t *operationStatus,
                                     unsigned long timeout = 100);

    /**
     * @brief Gets the current servo angle of a single GPIO.
     *
     * Retrieves the current angle setting of the servo on the specified GPIO channel.
     *
     * @param id Device position in the chain (starting from 1).
     * @param gpio The GPIO channel to query.
     * @param angle Pointer to store the retrieved angle.
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosAngle(uint8_t id, uint8_t gpio, uint8_t *angle, uint8_t *operationStatus,
                                  unsigned long timeout = 100);

    /**
     * @brief Gets the current servo angles of all GPIOs.
     *
     * Retrieves the current angle settings for all servos on the device.
     *
     * @param id Device position in the chain (starting from 1).
     * @param angle Pointer to a buffer where the angles will be stored.
     * @param size The number of channels to read.
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosAngleAll(uint8_t id, uint8_t *angle, uint8_t size, unsigned long timeout = 100);

    /**
     * @brief Sets the RGB LED configuration for a single GPIO.
     *
     * Configures RGB LED parameters (such as color order or timing) for the specified GPIO channel.
     * The channel must be in RGB mode.
     *
     * @param id Device position in the chain (starting from 1).
     * @param gpio The GPIO channel to configure.
     * @param config The configuration value to set.
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t setServosRGBConfig(uint8_t id, uint8_t gpio, uint8_t config, uint8_t *operationStatus,
                                      unsigned long timeout = 100);

    /**
     * @brief Sets the RGB LED configuration for all GPIOs.
     *
     * Batch configures RGB LED parameters for all GPIO channels.
     *
     * @param id Device position in the chain (starting from 1).
     * @param config Pointer to an array of RGB configuration data.
     * @param size The number of channels to configure.
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t setServosRGBConfigAll(uint8_t id, uint8_t *config, uint8_t size, uint8_t *operationStatus,
                                         unsigned long timeout = 100);

    /**
     * @brief Gets the RGB LED configuration of a single GPIO.
     *
     * Retrieves the current RGB configuration of the specified GPIO channel.
     *
     * @param id Device position in the chain (starting from 1).
     * @param gpio The GPIO channel to query.
     * @param config Pointer to store the retrieved RGB configuration.
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosRGBConfig(uint8_t id, uint8_t gpio, uint8_t *config, uint8_t *operationStatus,
                                      unsigned long timeout = 100);

    /**
     * @brief Gets the RGB LED configuration of all GPIOs.
     *
     * Retrieves the current RGB configurations for all GPIO channels.
     *
     * @param id Device position in the chain (starting from 1).
     * @param config Pointer to a buffer where the RGB configurations will be stored.
     * @param size The number of channels to read.
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosRGBConfigAll(uint8_t id, uint8_t *config, uint8_t size, unsigned long timeout = 100);

    /**
     * @brief Sets the RGB color buffer for a single LED index.
     *
     * Sends a 24-bit color value to the RGB LED at the specified index.
     *
     * @param id Device position in the chain (starting from 1).
     * @param index The index of the RGB LED to control.
     * @param buffer The 24-bit color value (e.g., 0xRRGGBB).
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t setServosRGBBuffer(uint8_t id, uint8_t index, uint32_t buffer, uint8_t *operationStatus,
                                      unsigned long timeout = 100);

    /**
     * @brief Sets the RGB color buffers for all LEDs.
     *
     * Sends an array of raw 24-bit color data to all RGB LEDs.
     *
     * @param id Device position in the chain (starting from 1).
     * @param buffer Pointer to an array of 24-bit color values.
     * @param size The number of LEDs to set.
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t setServosRGBBufferAll(uint8_t id, uint32_t *buffer, uint8_t size, unsigned long timeout = 100);

    /**
     * @brief Gets the RGB color buffer of a single LED index.
     *
     * Retrieves the current 24-bit color value from the specified RGB LED index.
     *
     * @param id Device position in the chain (starting from 1).
     * @param index The index of the RGB LED to query.
     * @param buffer Pointer to store the retrieved 24-bit color value.
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosRGBBuffer(uint8_t id, uint8_t index, uint32_t *buffer, uint8_t *operationStatus,
                                      unsigned long timeout = 100);

    /**
     * @brief Gets the RGB color buffers of all LEDs.
     *
     * Retrieves the current 24-bit color values from all RGB LEDs.
     *
     * @param id Device position in the chain (starting from 1).
     * @param buffer Pointer to store the retrieved color data array.
     * @param size The number of LEDs to read.
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosRGBBufferAll(uint8_t id, uint32_t *buffer, uint8_t size, unsigned long timeout = 100);

    /**
     * @brief Sets the PWM duty cycle for a single GPIO.
     *
     * Sets the PWM duty cycle for the specified GPIO channel. The channel must be in PWM mode.
     *
     * @param id Device position in the chain (starting from 1).
     * @param gpio The GPIO channel to control.
     * @param duty The duty cycle value (0-255).
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t setServosPwmDuty(uint8_t id, uint8_t gpio, uint8_t duty, uint8_t *operationStatus,
                                    unsigned long timeout = 100);

    /**
     * @brief Sets the PWM duty cycle for all GPIOs.
     *
     * Batch sets the PWM duty cycles for all GPIO channels.
     *
     * @param id Device position in the chain (starting from 1).
     * @param duty Pointer to an array containing duty cycle values for each channel.
     * @param size The number of channels to set.
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t setServosPwmDutyAll(uint8_t id, uint8_t *duty, uint8_t size, uint8_t *operationStatus,
                                       unsigned long timeout = 100);

    /**
     * @brief Gets the PWM duty cycle of a single GPIO.
     *
     * Retrieves the current PWM duty cycle of the specified GPIO channel.
     *
     * @param id Device position in the chain (starting from 1).
     * @param gpio The GPIO channel to query.
     * @param duty Pointer to store the retrieved duty cycle value.
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosPwmDuty(uint8_t id, uint8_t gpio, uint8_t *duty, uint8_t *operationStatus,
                                    unsigned long timeout = 100);

    /**
     * @brief Gets the PWM duty cycle of all GPIOs.
     *
     * Retrieves the current PWM duty cycles of all GPIO channels.
     *
     * @param id Device position in the chain (starting from 1).
     * @param duty Pointer to a buffer where the duty cycle values will be stored.
     * @param size The number of channels to read.
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosPwmDutyAll(uint8_t id, uint8_t *duty, uint8_t size, unsigned long timeout = 100);

    /**
     * @brief Sets the timer frequency for a single channel.
     *
     * Configures the frequency for the specified timer or GPIO channel.
     *
     * @param id Device position in the chain (starting from 1).
     * @param time The timer or channel index to configure.
     * @param freq The frequency value in Hz to set.
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t setServosTimeConfig(uint8_t id, uint8_t time, uint16_t freq, uint8_t *operationStatus,
                                       unsigned long timeout = 100);

    /**
     * @brief Sets the timer frequencies for all channels.
     *
     * Batch configures frequencies for all timer channels.
     *
     * @param id Device position in the chain (starting from 1).
     * @param freq Pointer to an array containing frequency values (in Hz) for each channel.
     * @param size The number of channels to configure.
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t setServosTimeConfigAll(uint8_t id, uint16_t *freq, uint8_t size, uint8_t *operationStatus,
                                          unsigned long timeout = 100);

    /**
     * @brief Gets the timer frequency of a single channel.
     *
     * Retrieves the current frequency configuration for a specific timer.
     *
     * @param id Device position in the chain (starting from 1).
     * @param time The timer or channel index to query.
     * @param frequency Pointer to store the retrieved frequency value (in Hz).
     * @param operationStatus Pointer to store the result of the operation, indicating the status:
     *                        - `CHAIN_UART_OPERATION_FAIL` for failure
     *                        - `CHAIN_UART_OPERATION_SUCCESS` for success
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosTimeConfig(uint8_t id, uint8_t time, uint16_t *frequency, uint8_t *operationStatus,
                                       unsigned long timeout = 100);

    /**
     * @brief Gets the timer frequencies of all channels.
     *
     * Retrieves the current frequency configurations for all available channels.
     *
     * @param id Device position in the chain (starting from 1).
     * @param frequency Pointer to a buffer where the frequency values will be stored.
     * @param size The number of channels to read.
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosTimeConfigAll(uint8_t id, uint16_t *frequency, uint8_t size, unsigned long timeout = 100);

    /**
     * @brief Gets the system reference voltage (VREF).
     *
     * Reads the internal reference voltage of the device in millivolts.
     *
     * @param id Device position in the chain (starting from 1).
     * @param vref Pointer to store the VREF value (in mV).
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosVref(uint8_t id, uint16_t *vref, unsigned long timeout = 100);

    /**
     * @brief Gets the Grove connector voltage.
     *
     * Reads the voltage level at the Grove interface in millivolts.
     *
     * @param id Device position in the chain (starting from 1).
     * @param voltage Pointer to store the voltage value (in mV).
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosGroveVoltage(uint8_t id, uint16_t *voltage, unsigned long timeout = 100);

    /**
     * @brief Gets the DC input voltage.
     *
     * Reads the voltage level of the external DC power supply in millivolts.
     *
     * @param id Device position in the chain (starting from 1).
     * @param voltage Pointer to store the voltage value (in mV).
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosDcVoltage(uint8_t id, uint16_t *voltage, unsigned long timeout = 100);

    /**
     * @brief Gets the total system current consumption.
     *
     * Reads the current consumption of the device in milliamperes.
     *
     * @param id Device position in the chain (starting from 1).
     * @param current Pointer to store the current value (in mA).
     * @param timeout Timeout in milliseconds for the operation (default is 100 ms).
     *
     * @return Returns the status of the operation.
     */
    chain_status_t getServosSysCurrent(uint8_t id, uint16_t *current, unsigned long timeout = 100);

private:
};

#endif  // _UNIT_8SERVOS2_CHAIN_HPP_

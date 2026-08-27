/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef _CHAIN_PEDAL_HPP_
#define _CHAIN_PEDAL_HPP_

#include <ChainCommon/ChainCommon.hpp>

/**
 * @brief Maximum RGB overlay value for the pedal device.
 */
#define PEDAL_OVERLAY_VALUE_MAX (0x0F)

/**
 * @brief Maximum configuration entry time in seconds.
 */
#define PEDAL_ENTER_CONFIG_TIME_MAX (30)

/**
 * @brief Enumeration of pedal working modes.
 */
typedef enum {
    CHAIN_PEDAL_SINGLE_MODE = 0, /**< Single-click mode. */
    CHAIN_PEDAL_SWITCH_MODE = 1, /**< Switch mode. */
} chain_pedal_work_mode_t;

/**
 * @brief Enumeration of pedal switch statuses.
 */
typedef enum {
    CHAIN_PEDAL_SWITCH_OFF_STATUS = 0, /**< Pedal switch is off. */
    CHAIN_PEDAL_SWITCH_ON_STATUS  = 1, /**< Pedal switch is on. */
} chain_pedal_switch_status_t;

/**
 * @brief Enumeration of pedal press events.
 */
typedef enum {
    CHAIN_PEDAL_SINGLE_CLICK = 0x0000, /**< Single-click event. */
    CHAIN_PEDAL_SWITCH_OFF   = 0x0400, /**< Switch-off event. */
    CHAIN_PEDAL_SWITCH_ON    = 0x0401, /**< Switch-on event. */
} chain_pedal_press_event_t;

/**
 * @brief Enumeration of pedal command codes.
 */
typedef enum {
    CHAIN_PEDAL_GET_BUTTON_STATUS     = 0xE1, /**< Get the current button status. */
    CHAIN_PEDAL_GET_SWITCH_STATUS     = 0xE2, /**< Get the current switch status. */
    CHAIN_PEDAL_SET_WORK_MODE         = 0xE3, /**< Set the working mode. */
    CHAIN_PEDAL_GET_WORK_MODE         = 0xE4, /**< Get the current working mode. */
    CHAIN_PEDAL_SET_BUTTON_MODE       = 0xE5, /**< Set the button report mode. */
    CHAIN_PEDAL_GET_BUTTON_MODE       = 0xE6, /**< Get the button report mode. */
    CHAIN_PEDAL_SET_ENTER_CONFIG_TIME = 0xE7, /**< Set the long-press configuration time. */
    CHAIN_PEDAL_GET_ENTER_CONFIG_TIME = 0xE8, /**< Get the long-press configuration time. */
    CHAIN_PEDAL_SET_RGB_OVERLAY_VALUE = 0xE9, /**< Set the RGB overlay mode. */
    CHAIN_PEDAL_GET_RGB_OVERLAY_VALUE = 0xEA, /**< Get the RGB overlay mode. */
} chain_pedal_cmd_t;

class ChainPedal : virtual public ChainCommon {
public:
    /**
     * @brief Retrieves the button status of a pedal device.
     *
     * This function reads the current button status of the pedal device at the specified
     * position in the chain.
     *
     * @param id             Position of the pedal device in the chain (1 = first device).
     * @param buttonStatus   Pointer to store the retrieved button status (0: released, 1: pressed).
     * @param timeout        Timeout in milliseconds (default: 100ms).
     *
     * @return chain_status_t Operation result: CHAIN_OK, CHAIN_BUSY, CHAIN_TIMEOUT, etc.
     */
    chain_status_t getPedalButtonStatus(uint8_t id, uint8_t *buttonStatus, unsigned long timeout = 100);

    /**
     * @brief Retrieves the switch status of a pedal device.
     *
     * This function reads the current ON/OFF status of the pedal's internal switch.
     *
     * @param id            Position of the pedal device in the chain (starting from 1).
     * @param switchStatus  Pointer to store the retrieved switch status.
     * @param timeout       Timeout in milliseconds (default: 100ms).
     *
     * @return chain_status_t Operation result: CHAIN_OK, CHAIN_BUSY, CHAIN_TIMEOUT, etc.
     */
    chain_status_t getPedalSwitchStatus(uint8_t id, chain_pedal_switch_status_t *switchStatus,
                                        unsigned long timeout = 100);

    /**
     * @brief Sets the pedal working mode.
     *
     * Selects single-click mode or switch mode and can optionally save the setting to flash.
     *
     * @param id              Device position in the chain (starting from 1).
     * @param workMode        Desired mode (single-click or switch).
     * @param operationStatus Operation result output (0: failure, 1: success).
     * @param saveToFlash     Save this setting to flash (default: disable).
     * @param timeout         Timeout in milliseconds (default: 100ms).
     *
     * @return Operation status.
     */
    chain_status_t setPedalWorkMode(uint8_t id, chain_pedal_work_mode_t workMode, uint8_t *operationStatus,
                                    chain_save_flash_t saveToFlash = CHAIN_SAVE_FLASH_DISABLE,
                                    unsigned long timeout          = 100);

    /**
     * @brief Retrieves the current working mode of a pedal device.
     *
     * @param id         Device position in the chain (starting from 1).
     * @param workMode   Pointer to store the retrieved working mode.
     * @param timeout    Timeout in milliseconds (default: 100ms).
     *
     * @return chain_status_t Operation result: CHAIN_OK, CHAIN_BUSY, CHAIN_TIMEOUT, etc.
     */
    chain_status_t getPedalWorkMode(uint8_t id, chain_pedal_work_mode_t *workMode, unsigned long timeout = 100);

    /**
     * @brief Sets the pedal button report mode.
     *
     * Enables or disables active reporting of pedal button events.
     *
     * @param id              Device position in the chain (starting from 1).
     * @param buttonMode      Desired report mode.
     * @param operationStatus Operation result output (0: failure, 1: success).
     * @param timeout         Timeout in milliseconds (default: 100ms).
     *
     * @return Operation status.
     */
    chain_status_t setPedalButtonMode(uint8_t id, chain_button_mode_t buttonMode, uint8_t *operationStatus,
                                      unsigned long timeout = 100);

    /**
     * @brief Retrieves the button report mode of a pedal device.
     *
     * @param id          Device position in the chain (starting from 1).
     * @param buttonMode  Pointer to store the retrieved button mode.
     * @param timeout     Timeout in milliseconds (default: 100ms).
     *
     * @return chain_status_t Operation result: CHAIN_OK, CHAIN_BUSY, CHAIN_TIMEOUT, etc.
     */
    chain_status_t getPedalButtonMode(uint8_t id, chain_button_mode_t *buttonMode, unsigned long timeout = 100);

    /**
     * @brief Sets the long-press time for entering pedal configuration mode.
     *
     * A value of 0 disables configuration entry. The setting can optionally be stored in flash.
     *
     * @param id              Device position in the chain (starting from 1).
     * @param enterConfigTime Long-press time in seconds (0-30).
     * @param operationStatus Operation result output (0: failure, 1: success).
     * @param saveToFlash     Save this setting to flash (default: disable).
     * @param timeout         Timeout in milliseconds (default: 100ms).
     *
     * @return Operation status.
     */
    chain_status_t setPedalEnterConfigTime(uint8_t id, uint8_t enterConfigTime, uint8_t *operationStatus,
                                           chain_save_flash_t saveToFlash = CHAIN_SAVE_FLASH_DISABLE,
                                           unsigned long timeout          = 100);

    /**
     * @brief Retrieves the long-press time for entering pedal configuration mode.
     *
     * @param id              Device position in the chain (starting from 1).
     * @param enterConfigTime Pointer to the long-press time in seconds.
     * @param timeout         Timeout in milliseconds (default: 100ms).
     *
     * @return Operation status.
     */
    chain_status_t getPedalEnterConfigTime(uint8_t id, uint8_t *enterConfigTime, unsigned long timeout = 100);

    /**
     * @brief Sets the RGB overlay mode of a pedal device.
     *
     * The low four bits control whether the button-state overlay is enabled for each RGB LED.
     * The setting can optionally be stored in flash memory.
     *
     * @param id              Device position in the chain (starting from 1).
     * @param overlayValue    RGB overlay bitmask (bits 0-3, one bit per RGB LED).
     * @param operationStatus Operation result output (0: failure, 1: success).
     * @param saveToFlash     Save this setting to flash (default: disable).
     * @param timeout         Timeout in milliseconds (default: 100ms).
     *
     * @return Operation status.
     */
    chain_status_t setPedalRGBOverlayValue(uint8_t id, uint8_t overlayValue, uint8_t *operationStatus,
                                           chain_save_flash_t saveToFlash = CHAIN_SAVE_FLASH_DISABLE,
                                           unsigned long timeout          = 100);

    /**
     * @brief Retrieves the RGB overlay mode of a pedal device.
     *
     * @param id            Device position in the chain (starting from 1).
     * @param overlayValue  Pointer to store the RGB overlay bitmask.
     * @param timeout       Timeout in milliseconds (default: 100ms).
     *
     * @return Operation status.
     */
    chain_status_t getPedalRGBOverlayValue(uint8_t id, uint8_t *overlayValue, unsigned long timeout = 100);

    /**
     * @brief Retrieves the latest trigger event of a pedal device.
     *
     * @param id             Device position in the chain (starting from 1).
     * @param triggerStatus  Pointer to store the retrieved press event.
     *
     * @return true if the trigger event was successfully read; false otherwise.
     */
    bool getPedalTriggerStatus(uint8_t id, chain_pedal_press_event_t *triggerStatus);

private:
};

#endif  // _CHAIN_PEDAL_HPP_

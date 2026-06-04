/*
 *SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#ifndef _CHAIN_MONO_HPP_
#define _CHAIN_MONO_HPP_

#include "ChainCommon/ChainCommon.hpp"

/**
 * @brief Mono device operation modes.
 */
typedef enum {
    MONO_PIXEL_MODE         = 0x00, /**< Pixel operation mode. */
    MONO_STRING_SCROLL_MODE = 0x01, /**< String scroll operation mode. */
} mono_mode_t;

/**
 * @brief Mono display rotation options.
 */
typedef enum {
    MONO_ROTATION_0   = 0, /**< 0° rotation. */
    MONO_ROTATION_90  = 1, /**< 90° clockwise rotation. */
    MONO_ROTATION_180 = 2, /**< 180° rotation. */
    MONO_ROTATION_270 = 3  /**< 270° clockwise rotation. */
} mono_rotation_t;

/**
 * @brief Mono display brightness levels.
 */
typedef enum {
    MONO_BRIGHTNESS_OFF     = 0, /**< 0% brightness (off). */
    MONO_BRIGHTNESS_LEVEL_1 = 1, /**< 3.1% brightness (very dim). */
    MONO_BRIGHTNESS_LEVEL_2 = 2, /**< 6.2% brightness (dim). */
    MONO_BRIGHTNESS_LEVEL_3 = 3, /**< 12.5% brightness (low). */
    MONO_BRIGHTNESS_LEVEL_4 = 4, /**< 25% brightness (medium-low). */
    MONO_BRIGHTNESS_LEVEL_5 = 5, /**< 43.75% brightness (medium). */
    MONO_BRIGHTNESS_LEVEL_6 = 6, /**< 68.75% brightness (high). */
    MONO_BRIGHTNESS_LEVEL_7 = 7, /**< 96.9% brightness (maximum). */
} mono_brightness_level_t;

/**
 * @brief Mono device command codes.
 */
typedef enum {
    CHAIN_MONO_SET_MODE              = 0x10, /**< Set operation mode. */
    CHAIN_MONO_GET_MODE              = 0x11, /**< Get current operation mode. */
    CHAIN_MONO_SET_PIXEL             = 0x30, /**< Set individual pixel state. */
    CHAIN_MONO_SET_DISPLAY_BUFFER    = 0x31, /**< Set entire display buffer. */
    CHAIN_MONO_GET_PIXEL             = 0x32, /**< Get individual pixel state. */
    CHAIN_MONO_GET_DISPLAY_BUFFER    = 0x33, /**< Get entire display buffer. */
    CHAIN_MONO_SET_CHAR              = 0x34, /**< Set character display. */
    CHAIN_MONO_SET_SCROLL_TEXT       = 0x40, /**< Set scroll text parameters. */
    CHAIN_MONO_GET_SCROLL_TEXT       = 0x41, /**< Get scroll text parameters. */
    CHAIN_MONO_SET_SCROLL_TEXT_STATE = 0x42, /**< Set scroll text state. */
    CHAIN_MONO_GET_SCROLL_TEXT_STATE = 0x43, /**< Get scroll text state. */
    CHAIN_MONO_SET_ROTATION          = 0xE0, /**< Set display rotation. */
    CHAIN_MONO_GET_ROTATION          = 0xE1, /**< Get display rotation. */
    CHAIN_MONO_SET_BRIGHTNESS        = 0xE2, /**< Set display brightness. */
    CHAIN_MONO_GET_BRIGHTNESS        = 0xE3, /**< Get display brightness. */
    CHAIN_MONO_CLEAR                 = 0xE4, /**< Clear display. */
} chain_mono_cmd_t;

/**
 * @brief Pixel information structure.
 */
struct MonoPixelInfo {
    uint8_t x;  /**< X coordinate of the pixel. */
    uint8_t y;  /**< Y coordinate of the pixel. */
    bool state; /**< State of the pixel (true = on, false = off). */
};

/**
 * @brief Scroll animation modes.
 */
typedef enum {
    MONO_SCROLL_MODE_ONCE   = 0, /**< Scroll once and stop. */
    MONO_SCROLL_MODE_LOOP   = 1, /**< Scroll continuously in loop. */
    MONO_SCROLL_MODE_BOUNCE = 2  /**< Scroll back and forth. */
} mono_scroll_mode_t;

/**
 * @brief Scroll direction options.
 */
typedef enum {
    MONO_SCROLL_LEFT  = 0, /**< Scroll to the left. */
    MONO_SCROLL_RIGHT = 1, /**< Scroll to the right. */
    MONO_SCROLL_UP    = 2, /**< Scroll upward. */
    MONO_SCROLL_DOWN  = 3  /**< Scroll downward. */
} mono_scroll_dir_t;

/**
 * @brief Scroll state enumeration.
 */
typedef enum {
    MONO_SCROLL_STATE_RUNNING = 0, /**< Scroll is running. */
    MONO_SCROLL_STATE_PAUSED  = 1, /**< Scroll is paused. */
    MONO_SCROLL_STATE_IDLE    = 2  /**< Scroll is idle/stopped. */
} mono_scroll_state_t;

/**
 * @brief ChainMono class for controlling Mono devices in the chain.
 */
class ChainMono : virtual public ChainCommon {
public:
    /**
     * @brief Sets the operation mode of the Mono device.
     *
     * @param id Device position in the chain (starting from 1).
     * @param mode Operation mode to set.
     * @param operationStatus Pointer to store the device operation status.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setMonoMode(uint8_t id, mono_mode_t mode, uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Gets the current operation mode of the Mono device.
     *
     * @param id Device position in the chain (starting from 1).
     * @param mode Pointer to store the current operation mode.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t getMonoMode(uint8_t id, mono_mode_t *mode, unsigned long timeout = 100);

    /**
     * @brief Sets the state of multiple pixels on the Mono display.
     *
     * @param id Device position in the chain (starting from 1).
     * @param pixels Pointer to an array of PixelInfo structures containing pixel coordinates and states.
     * @param count Number of pixels to set.
     * @param operationStatus Pointer to store the device operation status.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setMonoPixel(uint8_t id, MonoPixelInfo *pixels, uint8_t count, uint8_t *operationStatus,
                                unsigned long timeout = 100);

    /**
     * @brief Sets the state of a single pixel on the Mono display.
     *
     * @param id Device position in the chain (starting from 1).
     * @param x X coordinate of the pixel.
     * @param y Y coordinate of the pixel.
     * @param state State to set (true = on, false = off).
     * @param operationStatus Pointer to store the device operation status.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setMonoPixel(uint8_t id, uint8_t x, uint8_t y, bool state, uint8_t *operationStatus,
                                unsigned long timeout = 100);

    /**
     * @brief Gets the state of multiple pixels from the Mono display.
     *
     * @param id Device position in the chain (starting from 1).
     * @param pixels Pointer to an array of PixelInfo structures to store pixel coordinates and states.
     * @param count Number of pixels to get.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t getMonoPixel(uint8_t id, MonoPixelInfo *pixels, uint8_t count, unsigned long timeout = 100);

    /**
     * @brief Gets the state of a single pixel from the Mono display.
     *
     * @param id Device position in the chain (starting from 1).
     * @param x X coordinate of the pixel.
     * @param y Y coordinate of the pixel.
     * @param state Pointer to store the pixel state (true = on, false = off).
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t getMonoPixel(uint8_t id, uint8_t x, uint8_t y, bool *state, unsigned long timeout = 100);

    /**
     * @brief Sets and refreshes the entire display buffer of the Mono device.
     *
     * @param id Device position in the chain (starting from 1).
     * @param buffer Reference to an 8-byte array containing the display buffer data.
     * @param operationStatus Pointer to store the device operation status.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setMonoBufferRefresh(uint8_t id, uint8_t (&buffer)[8], uint8_t *operationStatus,
                                        unsigned long timeout = 100);

    /**
     * @brief Gets the current display buffer from the Mono device.
     *
     * @param id Device position in the chain (starting from 1).
     * @param buffer Pointer to store the display buffer data.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t getMonoRefreshBuffer(uint8_t id, uint8_t *buffer, unsigned long timeout = 100);

    /**
     * @brief Prints a character on the Mono display at the specified position.
     *
     * @param id Device position in the chain (starting from 1).
     * @param ascii ASCII character to display.
     * @param x X coordinate of the character position.
     * @param y Y coordinate of the character position.
     * @param operationStatus Pointer to store the device operation status.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setMonoPrintChar(uint8_t id, char ascii, uint8_t x, uint8_t y, uint8_t *operationStatus,
                                    unsigned long timeout = 100);

    /**
     * @brief Sets up scroll text parameters for the Mono display.
     *
     * @param id Device position in the chain (starting from 1).
     * @param string Text string to scroll.
     * @param dir Direction of the scroll.
     * @param mode Scroll animation mode.
     * @param IntervalMs Scroll interval in milliseconds per step.
     * @param operationStatus Pointer to store the device operation status.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setMonoStringScroll(uint8_t id, const char *string, mono_scroll_dir_t dir, mono_scroll_mode_t mode,
                                       uint16_t IntervalMs, uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Gets the current scroll text parameters from the Mono device.
     *
     * @param id Device position in the chain (starting from 1).
     * @param string Pointer to store the current scroll text.
     * @param dir Pointer to store the current scroll direction.
     * @param mode Pointer to store the current scroll mode.
     * @param IntervalMs Pointer to store the current scroll interval in milliseconds per step.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t getMonoStringScroll(uint8_t id, char *string, mono_scroll_dir_t *dir, mono_scroll_mode_t *mode,
                                       uint16_t *IntervalMs, unsigned long timeout = 100);

    /**
     * @brief Sets the scroll text state on the Mono device.
     *
     * @param id Device position in the chain (starting from 1).
     * @param state Scroll state to set (running, paused, idle).
     * @param operationStatus Pointer to store the device operation status.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setMonoStringScrollState(uint8_t id, mono_scroll_state_t state, uint8_t *operationStatus,
                                            unsigned long timeout = 100);

    /**
     * @brief Gets the current scroll text state from the Mono device.
     *
     * @param id Device position in the chain (starting from 1).
     * @param state Pointer to store the current scroll state.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t getMonoStringScrollState(uint8_t id, mono_scroll_state_t *state, unsigned long timeout = 100);

    /**
     * @brief Sets the display rotation of the Mono device.
     *
     * @param id Device position in the chain (starting from 1).
     * @param rotation Rotation angle to set.
     * @param operationStatus Pointer to store the device operation status.
     * @param saveToFlash Whether to save the setting to flash memory.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setMonoRotation(uint8_t id, mono_rotation_t rotation, uint8_t *operationStatus,
                                   chain_save_flash_t saveToFlash = CHAIN_SAVE_FLASH_DISABLE,
                                   unsigned long timeout          = 100);

    /**
     * @brief Gets the current display rotation from the Mono device.
     *
     * @param id Device position in the chain (starting from 1).
     * @param rotation Pointer to store the current rotation angle.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t getMonoRotation(uint8_t id, mono_rotation_t *rotation, unsigned long timeout = 100);

    /**
     * @brief Sets the brightness of the Mono display.
     *
     * @param id Device position in the chain (starting from 1).
     * @param brightness Brightness level to set.
     * @param operationStatus Pointer to store the device operation status.
     * @param saveToFlash Whether to save the setting to flash memory.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setMonoBrightness(uint8_t id, mono_brightness_level_t brightness, uint8_t *operationStatus,
                                     chain_save_flash_t saveToFlash = CHAIN_SAVE_FLASH_DISABLE,
                                     unsigned long timeout          = 100);

    /**
     * @brief Gets the current brightness from the Mono device.
     *
     * @param id Device position in the chain (starting from 1).
     * @param brightness Pointer to store the current brightness level.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t getMonoBrightness(uint8_t id, mono_brightness_level_t *brightness, unsigned long timeout = 100);

    /**
     * @brief Clears the Mono display.
     *
     * @param id Device position in the chain (starting from 1).
     * @param operationStatus Pointer to store the device operation status.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setMonoClear(uint8_t id, uint8_t *operationStatus, unsigned long timeout = 100);

private:
};

#endif  // _CHAIN_MONO_HPP_

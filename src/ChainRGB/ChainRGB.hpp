/*
 *SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#ifndef _CHAIN_RGB_HPP_
#define _CHAIN_RGB_HPP_

#include <cstdint>
#include "ChainCommon/ChainCommon.hpp"

/**
 * @brief Macro for colorful gradient effect in string scroll mode.
 */
#define RGB_SCROLL_COLOR_GRADIENT 0 /**< Color value for colorful gradient effect in string scroll mode. */

/**
 * @brief RGB device operation modes.
 */
typedef enum {
    RGB_PIXEL_MODE         = 0x00, /**< Pixel operation mode. */
    RGB_STRING_SCROLL_MODE = 0x01, /**< String scroll operation mode. */
} rgb_mode_t;

/**
 * @brief RGB display rotation options.
 */
typedef enum {
    RGB_ROTATION_0   = 0, /**< 0° rotation. */
    RGB_ROTATION_90  = 1, /**< 90° clockwise rotation. */
    RGB_ROTATION_180 = 2, /**< 180° rotation. */
    RGB_ROTATION_270 = 3  /**< 270° clockwise rotation. */
} rgb_rotation_t;

/**
 * @brief RGB device command codes.
 */
typedef enum {
    CHAIN_RGB_SET_MODE              = 0x10, /**< Set operation mode. */
    CHAIN_RGB_GET_MODE              = 0x11, /**< Get current operation mode. */
    CHAIN_RGB_SET_PIXEL             = 0x30, /**< Set individual pixel state. */
    CHAIN_RGB_SET_DISPLAY_BUFFER    = 0x31, /**< Set entire display buffer. */
    CHAIN_RGB_GET_PIXEL             = 0x32, /**< Get individual pixel state. */
    CHAIN_RGB_GET_DISPLAY_BUFFER    = 0x33, /**< Get entire display buffer. */
    CHAIN_RGB_SET_CHAR              = 0x34, /**< Set character display. */
    CHAIN_RGB_SET_SCROLL_TEXT       = 0x40, /**< Set scroll text parameters. */
    CHAIN_RGB_GET_SCROLL_TEXT       = 0x41, /**< Get scroll text parameters. */
    CHAIN_RGB_SET_SCROLL_TEXT_STATE = 0x42, /**< Set scroll text state. */
    CHAIN_RGB_GET_SCROLL_TEXT_STATE = 0x43, /**< Get scroll text state. */
    CHAIN_RGB_SET_ROTATION          = 0xE0, /**< Set display rotation. */
    CHAIN_RGB_GET_ROTATION          = 0xE1, /**< Get display rotation. */
    CHAIN_RGB_SET_BRIGHTNESS        = 0xE2, /**< Set display brightness. */
    CHAIN_RGB_GET_BRIGHTNESS        = 0xE3, /**< Get display brightness. */
    CHAIN_RGB_CLEAR                 = 0xE4, /**< Clear display. */
} chain_rgb_cmd_t;

/**
 * @brief Pixel information structure.
 */
struct RGBPixelInfo {
    uint8_t x;      /**< X coordinate of the pixel. */
    uint8_t y;      /**< Y coordinate of the pixel. */
    uint16_t color; /**< Color of the pixel (RGB565 format). */
};

/**
 * @brief Scroll animation modes.
 */
typedef enum {
    RGB_SCROLL_MODE_ONCE   = 0, /**< Scroll once and stop. */
    RGB_SCROLL_MODE_LOOP   = 1, /**< Scroll continuously in loop. */
    RGB_SCROLL_MODE_BOUNCE = 2  /**< Scroll back and forth. */
} rgb_scroll_mode_t;

/**
 * @brief Scroll direction options.
 */
typedef enum {
    RGB_SCROLL_LEFT  = 0, /**< Scroll to the left. */
    RGB_SCROLL_RIGHT = 1, /**< Scroll to the right. */
    RGB_SCROLL_UP    = 2, /**< Scroll upward. */
    RGB_SCROLL_DOWN  = 3  /**< Scroll downward. */
} rgb_scroll_dir_t;

/**
 * @brief Scroll state enumeration.
 */
typedef enum {
    RGB_SCROLL_STATE_RUNNING = 0, /**< Scroll is running. */
    RGB_SCROLL_STATE_PAUSED  = 1, /**< Scroll is paused. */
    RGB_SCROLL_STATE_IDLE    = 2  /**< Scroll is idle/stopped. */
} rgb_scroll_state_t;

/**
 * @brief ChainRGB class for controlling RGB devices in the chain.
 */
class ChainRGB : virtual public ChainCommon {
public:
    /**
     * @brief Sets the operation mode of the RGB device.
     *
     * @param id Device position in the chain (starting from 1).
     * @param mode Operation mode to set.
     * @param operationStatus Pointer to store the device operation status.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setRGBMode(uint8_t id, rgb_mode_t mode, uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Gets the current operation mode of the RGB device.
     *
     * @param id Device position in the chain (starting from 1).
     * @param mode Pointer to store the current operation mode.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t getRGBMode(uint8_t id, rgb_mode_t *mode, unsigned long timeout = 100);

    /**
     * @brief Sets the state of multiple pixels on the RGB display.
     *
     * @param id Device position in the chain (starting from 1).
     * @param pixels Pointer to an array of RGBPixelInfo structures containing pixel coordinates and states.
     * @param count Number of pixels to set.
     * @param operationStatus Pointer to store the device operation status.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setRGBPixel(uint8_t id, RGBPixelInfo *pixels, uint8_t count, uint8_t *operationStatus,
                               unsigned long timeout = 100);

    /**
     * @brief Sets the state of a single pixel on the RGB display.
     *
     * @param id Device position in the chain (starting from 1).
     * @param x X coordinate of the pixel.
     * @param y Y coordinate of the pixel.
     * @param color Color to set (RGB565 format).
     * @param operationStatus Pointer to store the device operation status.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setRGBPixel(uint8_t id, uint8_t x, uint8_t y, uint16_t color, uint8_t *operationStatus,
                               unsigned long timeout = 100);

    /**
     * @brief Gets the state of multiple pixels from the RGB display.
     *
     * @param id Device position in the chain (starting from 1).
     * @param pixels Pointer to an array of RGBPixelInfo structures to store pixel coordinates and states.
     * @param count Number of pixels to get.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t getRGBPixel(uint8_t id, RGBPixelInfo *pixels, uint8_t count, unsigned long timeout = 100);

    /**
     * @brief Gets the state of a single pixel from the RGB display.
     *
     * @param id Device position in the chain (starting from 1).
     * @param x X coordinate of the pixel.
     * @param y Y coordinate of the pixel.
     * @param color Pointer to store the pixel color (RGB565 format).
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t getRGBPixel(uint8_t id, uint8_t x, uint8_t y, uint16_t *color, unsigned long timeout = 100);

    /**
     * @brief Sets and refreshes the entire display buffer of the RGB device.
     *
     * @param id Device position in the chain (starting from 1).
     * @param buffer Reference to an 8-byte array containing the display buffer data.
     * @param operationStatus Pointer to store the device operation status.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setRGBBufferRefresh(uint8_t id, uint16_t (&buffer)[64], uint8_t *operationStatus,
                                       unsigned long timeout = 100);

    /**
     * @brief Gets the current display buffer from the RGB device.
     *
     * @param id Device position in the chain (starting from 1).
     * @param buffer Pointer to store the display buffer data.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t getRGBRefreshBuffer(uint8_t id, uint16_t *buffer, unsigned long timeout = 100);

    /**
     * @brief Prints a character on the RGB display at the specified position.
     *
     * @param id Device position in the chain (starting from 1).
     * @param ascii ASCII character to display.
     * @param x X coordinate of the character position.
     * @param y Y coordinate of the character position.
     * @param color Color to set (RGB565 format).
     * @param operationStatus Pointer to store the device operation status.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setRGBPrintChar(uint8_t id, char ascii, uint8_t x, uint8_t y, uint16_t color,
                                   uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Sets up scroll text parameters for the RGB display.
     *
     * @param id Device position in the chain (starting from 1).
     * @param string Text string to scroll (maximum 32 characters).
     * @param dir Direction of the scroll.
     * @param mode Scroll animation mode.
     * @param IntervalMs Scroll interval in milliseconds per step.
     * @param color Color to set (RGB565 format). Set to 0 or RGB_SCROLL_COLOR_GRADIENT for colorful gradient effect.
     * @param operationStatus Pointer to store the device operation status.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setRGBStringScroll(uint8_t id, const char *string, rgb_scroll_dir_t dir, rgb_scroll_mode_t mode,
                                      uint16_t IntervalMs, uint16_t color, uint8_t *operationStatus,
                                      unsigned long timeout = 100);

    /**
     * @brief Gets the current scroll text parameters from the RGB device.
     *
     * @param id Device position in the chain (starting from 1).
     * @param string Pointer to store the current scroll text. Buffer must be at least 33 bytes, including '\0'.
     * @param dir Pointer to store the current scroll direction.
     * @param mode Pointer to store the current scroll mode.
     * @param IntervalMs Pointer to store the current scroll interval in milliseconds per step.
     * @param color Pointer to store the current scroll color (RGB565 format). 0 or RGB_SCROLL_COLOR_GRADIENT indicates
     * colorful gradient effect.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t getRGBStringScroll(uint8_t id, char *string, rgb_scroll_dir_t *dir, rgb_scroll_mode_t *mode,
                                      uint16_t *IntervalMs, uint16_t *color, unsigned long timeout = 100);

    /**
     * @brief Sets the scroll text state on the RGB device.
     *
     * @param id Device position in the chain (starting from 1).
     * @param state Scroll state to set (running, paused, idle).
     * @param operationStatus Pointer to store the device operation status.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setRGBStringScrollState(uint8_t id, rgb_scroll_state_t state, uint8_t *operationStatus,
                                           unsigned long timeout = 100);

    /**
     * @brief Gets the current scroll text state from the RGB device.
     *
     * @param id Device position in the chain (starting from 1).
     * @param state Pointer to store the current scroll state.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t getRGBStringScrollState(uint8_t id, rgb_scroll_state_t *state, unsigned long timeout = 100);

    /**
     * @brief Sets the display rotation of the RGB device.
     *
     * @param id Device position in the chain (starting from 1).
     * @param rotation Rotation angle to set.
     * @param operationStatus Pointer to store the device operation status.
     * @param saveToFlash Whether to save the setting to flash memory.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setRGBRotation(uint8_t id, rgb_rotation_t rotation, uint8_t *operationStatus,
                                  chain_save_flash_t saveToFlash = CHAIN_SAVE_FLASH_DISABLE,
                                  unsigned long timeout          = 100);

    /**
     * @brief Gets the current display rotation from the RGB device.
     *
     * @param id Device position in the chain (starting from 1).
     * @param rotation Pointer to store the current rotation angle.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t getRGBRotation(uint8_t id, rgb_rotation_t *rotation, unsigned long timeout = 100);

    /**
     * @brief Sets the brightness of the RGB display.
     *
     * @param id Device position in the chain (starting from 1).
     * @param brightness Brightness value to set (0-100).
     * @param operationStatus Pointer to store the device operation status.
     * @param saveToFlash Whether to save the setting to flash memory.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setRGBBrightness(uint8_t id, uint8_t brightness, uint8_t *operationStatus,
                                    chain_save_flash_t saveToFlash = CHAIN_SAVE_FLASH_DISABLE,
                                    unsigned long timeout          = 100);

    /**
     * @brief Gets the current brightness from the RGB device.
     *
     * @param id Device position in the chain (starting from 1).
     * @param brightness Pointer to store the current brightness value(0-100).
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t getRGBBrightness(uint8_t id, uint8_t *brightness, unsigned long timeout = 100);

    /**
     * @brief Clears the RGB display.
     *
     * @param id Device position in the chain (starting from 1).
     * @param operationStatus Pointer to store the device operation status.
     * @param timeout Timeout duration for the operation in milliseconds.
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setRGBClear(uint8_t id, uint8_t *operationStatus, unsigned long timeout = 100);

private:
};

#endif  // _CHAIN_RGB_HPP_

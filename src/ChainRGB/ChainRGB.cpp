/*
 *SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#include "ChainRGB/ChainRGB.hpp"

chain_status_t ChainRGB::setRGBMode(uint8_t id, rgb_mode_t mode, uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)mode;
        sendPacket(id, CHAIN_RGB_SET_MODE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_RGB_SET_MODE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainRGB::getRGBMode(uint8_t id, rgb_mode_t *mode, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_RGB_GET_MODE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_RGB_GET_MODE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *mode = (rgb_mode_t)returnPacket[6];
            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainRGB::setRGBPixel(uint8_t id, RGBPixelInfo *pixels, uint8_t count, uint8_t *operationStatus,
                                     unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (!pixels || count == 0 || count > 64) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = count;
        for (int i = 0; i < count; i++) {
            uint8_t byteVal = 0;

            byteVal |= ((pixels[i].x & 0x07) << 3);
            byteVal |= (pixels[i].y & 0x07);

            cmdBuffer[cmdBufferSize++] = byteVal;
            cmdBuffer[cmdBufferSize++] = pixels[i].color & 0xFF;
            cmdBuffer[cmdBufferSize++] = (pixels[i].color >> 8) & 0xFF;
        }
        sendPacket(id, CHAIN_RGB_SET_PIXEL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_RGB_SET_PIXEL, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainRGB::setRGBPixel(uint8_t id, uint8_t x, uint8_t y, uint16_t color, uint8_t *operationStatus,
                                     unsigned long timeout)
{
    RGBPixelInfo p;
    p.x     = x;
    p.y     = y;
    p.color = color;
    return ChainRGB::setRGBPixel(id, &p, 1, operationStatus, timeout);
}

chain_status_t ChainRGB::getRGBPixel(uint8_t id, RGBPixelInfo *pixels, uint8_t count, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (!pixels || count == 0 || count > 64) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = count;

        for (uint8_t i = 0; i < count; i++) {
            uint8_t byteVal = 0;
            byteVal |= ((pixels[i].x & 0x07) << 3);
            byteVal |= (pixels[i].y & 0x07);
            cmdBuffer[cmdBufferSize++] = byteVal;
        }

        sendPacket(id, CHAIN_RGB_GET_PIXEL, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_RGB_GET_PIXEL, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                for (uint8_t i = 0; i < count; i++) {
                    pixels[i].color = returnPacket[6 + i * 2] | (returnPacket[7 + i * 2] << 8);
                }

            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }

        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainRGB::getRGBPixel(uint8_t id, uint8_t x, uint8_t y, uint16_t *color, unsigned long timeout)
{
    RGBPixelInfo p;
    p.x = x;
    p.y = y;

    chain_status_t ret = getRGBPixel(id, &p, 1, timeout);
    if (ret == CHAIN_OK && color) {
        *color = p.color;
    }
    return ret;
}

chain_status_t ChainRGB::setRGBBufferRefresh(uint8_t id, uint16_t (&buffer)[64], uint8_t *operationStatus,
                                             unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (acquireMutex()) {
        cmdBufferSize = 0;
        for (int i = 0; i < 64; i++) {
            cmdBuffer[cmdBufferSize++] = buffer[i] & 0xFF;
            cmdBuffer[cmdBufferSize++] = (buffer[i] >> 8) & 0xFF;
        }
        sendPacket(id, CHAIN_RGB_SET_DISPLAY_BUFFER, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_RGB_SET_DISPLAY_BUFFER, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainRGB::getRGBRefreshBuffer(uint8_t id, uint16_t *buffer, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_RGB_GET_DISPLAY_BUFFER, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_RGB_GET_DISPLAY_BUFFER, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                for (int i = 0; i < 64; i++) {
                    buffer[i] = returnPacket[6 + i * 2] | (returnPacket[7 + i * 2] << 8);
                }

            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainRGB::setRGBPrintChar(uint8_t id, char ascii, uint8_t x, uint8_t y, uint16_t color,
                                         uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)ascii;
        uint8_t byteVal            = 0;
        byteVal |= ((x & 0x07) << 4);
        byteVal |= (y & 0x07);
        cmdBuffer[cmdBufferSize++] = byteVal;
        cmdBuffer[cmdBufferSize++] = color & 0xFF;
        cmdBuffer[cmdBufferSize++] = (color >> 8) & 0xFF;
        sendPacket(id, CHAIN_RGB_SET_CHAR, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_RGB_SET_CHAR, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainRGB::setRGBStringScroll(uint8_t id, const char *string, rgb_scroll_dir_t dir,
                                            rgb_scroll_mode_t mode, uint16_t IntervalMs, uint16_t color,
                                            uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (!string) {
        return CHAIN_PARAMETER_ERROR;
    }

    size_t length              = strlen(string);
    const size_t maxTextLength = SEND_BUFFER_SIZE - 9 - 6;  // send packet overhead + scroll config bytes
    if (length > maxTextLength) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize   = 0;
        uint8_t byteVal = 0;
        byteVal |= ((dir & 0x0F) << 4);
        byteVal |= (uint8_t)mode & 0x0F;
        cmdBuffer[cmdBufferSize++] = byteVal;
        cmdBuffer[cmdBufferSize++] = (uint8_t)IntervalMs & 0xFF;
        cmdBuffer[cmdBufferSize++] = (uint8_t)(IntervalMs >> 8) & 0xFF;
        cmdBuffer[cmdBufferSize++] = (uint8_t)color & 0xFF;
        cmdBuffer[cmdBufferSize++] = (uint8_t)(color >> 8) & 0xFF;
        cmdBuffer[cmdBufferSize++] = (uint8_t)length;
        for (size_t i = 0; i < length; i++) {
            cmdBuffer[cmdBufferSize++] = (uint8_t)string[i];
        }
        sendPacket(id, CHAIN_RGB_SET_SCROLL_TEXT, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_RGB_SET_SCROLL_TEXT, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }

        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainRGB::getRGBStringScroll(uint8_t id, char *string, rgb_scroll_dir_t *dir, rgb_scroll_mode_t *mode,
                                            uint16_t *IntervalMs, uint16_t *color, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (!string || !dir || !mode || !IntervalMs || !color) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_RGB_GET_SCROLL_TEXT, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_RGB_GET_SCROLL_TEXT, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                if (returnPacketSize < 15) {
                    status = CHAIN_RETURN_PACKET_ERROR;
                } else {
                    *dir           = (rgb_scroll_dir_t)((returnPacket[6] >> 4) & 0x0F);
                    *mode          = (rgb_scroll_mode_t)(returnPacket[6] & 0x0F);
                    *IntervalMs    = (uint16_t)(returnPacket[7] | (returnPacket[8] << 8));
                    *color         = (uint16_t)(returnPacket[9] | (returnPacket[10] << 8));
                    uint8_t length = returnPacket[11];
                    if (returnPacketSize < 15 + length) {
                        status = CHAIN_RETURN_PACKET_ERROR;
                    } else {
                        for (uint8_t i = 0; i < length; i++) {
                            string[i] = returnPacket[12 + i];
                        }
                        string[length] = '\0';
                    }
                }

            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainRGB::setRGBStringScrollState(uint8_t id, rgb_scroll_state_t state, uint8_t *operationStatus,
                                                 unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)state;
        sendPacket(id, CHAIN_RGB_SET_SCROLL_TEXT_STATE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_RGB_SET_SCROLL_TEXT_STATE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainRGB::getRGBStringScrollState(uint8_t id, rgb_scroll_state_t *state, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_RGB_GET_SCROLL_TEXT_STATE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_RGB_GET_SCROLL_TEXT_STATE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *state = (rgb_scroll_state_t)returnPacket[6];
            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainRGB::setRGBRotation(uint8_t id, rgb_rotation_t rotation, uint8_t *operationStatus,
                                        chain_save_flash_t saveToFlash, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)rotation;
        cmdBuffer[cmdBufferSize++] = saveToFlash;
        sendPacket(id, CHAIN_RGB_SET_ROTATION, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_RGB_SET_ROTATION, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainRGB::getRGBRotation(uint8_t id, rgb_rotation_t *rotation, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_RGB_GET_ROTATION, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_RGB_GET_ROTATION, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *rotation = (rgb_rotation_t)returnPacket[6];
            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainRGB::setRGBBrightness(uint8_t id, uint8_t brightness, uint8_t *operationStatus,
                                          chain_save_flash_t saveToFlash, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = brightness;
        cmdBuffer[cmdBufferSize++] = saveToFlash;
        sendPacket(id, CHAIN_RGB_SET_BRIGHTNESS, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_RGB_SET_BRIGHTNESS, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainRGB::getRGBBrightness(uint8_t id, uint8_t *brightness, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_RGB_GET_BRIGHTNESS, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_RGB_GET_BRIGHTNESS, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *brightness = returnPacket[6];
            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

chain_status_t ChainRGB::setRGBClear(uint8_t id, uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_RGB_CLEAR, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_RGB_CLEAR, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
            } else {
                status = CHAIN_RETURN_PACKET_ERROR;
            }
        } else {
            status = CHAIN_TIMEOUT;
        }
        releaseMutex();
    } else {
        status = CHAIN_BUSY;
    }

    return status;
}

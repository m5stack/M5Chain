/*
 *SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#include "ChainMono/ChainMono.hpp"

chain_status_t ChainMono::setMonoMode(uint8_t id, mono_mode_t mode, uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)mode;
        sendPacket(id, CHAIN_MONO_SET_MODE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_MONO_SET_MODE, timeout)) {
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

chain_status_t ChainMono::getMonoMode(uint8_t id, mono_mode_t *mode, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_MONO_GET_MODE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_MONO_GET_MODE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *mode = (mono_mode_t)returnPacket[6];
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

chain_status_t ChainMono::setMonoPixel(uint8_t id, MonoPixelInfo *pixels, uint8_t count, uint8_t *operationStatus,
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

            if (pixels[i].state) {
                byteVal |= (1 << 6);
            }
            byteVal |= ((pixels[i].x & 0x07) << 3);
            byteVal |= (pixels[i].y & 0x07);

            cmdBuffer[cmdBufferSize++] = byteVal;
        }
        sendPacket(id, CHAIN_MONO_SET_PIXEL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_MONO_SET_PIXEL, timeout)) {
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

chain_status_t ChainMono::setMonoPixel(uint8_t id, uint8_t x, uint8_t y, bool state, uint8_t *operationStatus,
                                       unsigned long timeout)
{
    MonoPixelInfo p;
    p.x     = x;
    p.y     = y;
    p.state = state;
    return ChainMono::setMonoPixel(id, &p, 1, operationStatus, timeout);
}

chain_status_t ChainMono::getMonoPixel(uint8_t id, MonoPixelInfo *pixels, uint8_t count, unsigned long timeout)
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

        sendPacket(id, CHAIN_MONO_GET_PIXEL, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_MONO_GET_PIXEL, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                for (uint8_t i = 0; i < count; i++) {
                    pixels[i].state = returnPacket[6 + i] ? true : false;
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

chain_status_t ChainMono::getMonoPixel(uint8_t id, uint8_t x, uint8_t y, bool *state, unsigned long timeout)
{
    MonoPixelInfo p;
    p.x = x;
    p.y = y;

    chain_status_t ret = getMonoPixel(id, &p, 1, timeout);
    if (ret == CHAIN_OK && state) {
        *state = p.state;
    }
    return ret;
}

chain_status_t ChainMono::setMonoBufferRefresh(uint8_t id, uint8_t (&buffer)[8], uint8_t *operationStatus,
                                               unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (acquireMutex()) {
        cmdBufferSize = 0;
        for (int i = 0; i < 8; i++) {
            cmdBuffer[cmdBufferSize++] = buffer[i];
        }
        sendPacket(id, CHAIN_MONO_SET_DISPLAY_BUFFER, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_MONO_SET_DISPLAY_BUFFER, timeout)) {
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

chain_status_t ChainMono::getMonoRefreshBuffer(uint8_t id, uint8_t *buffer, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_MONO_GET_DISPLAY_BUFFER, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_MONO_GET_DISPLAY_BUFFER, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                for (int i = 0; i < 8; i++) {
                    buffer[i] = returnPacket[6 + i];
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

chain_status_t ChainMono::setMonoPrintChar(uint8_t id, char ascii, uint8_t x, uint8_t y, uint8_t *operationStatus,
                                           unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)ascii;
        uint8_t byteVal            = 0;
        byteVal |= ((x & 0x07) << 4);
        byteVal |= (y & 0x07);
        cmdBuffer[cmdBufferSize++] = byteVal;
        sendPacket(id, CHAIN_MONO_SET_CHAR, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_MONO_SET_CHAR, timeout)) {
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

chain_status_t ChainMono::setMonoStringScroll(uint8_t id, const char *string, mono_scroll_dir_t dir,
                                              mono_scroll_mode_t mode, uint16_t IntervalMs, uint8_t *operationStatus,
                                              unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (!string) {
        return CHAIN_PARAMETER_ERROR;
    }

    size_t length              = strlen(string);
    const size_t maxTextLength = SEND_BUFFER_SIZE - 9 - 4;  // send packet overhead + scroll config bytes
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
        cmdBuffer[cmdBufferSize++] = (uint8_t)length;
        for (size_t i = 0; i < length; i++) {
            cmdBuffer[cmdBufferSize++] = (uint8_t)string[i];
        }
        sendPacket(id, CHAIN_MONO_SET_SCROLL_TEXT, cmdBuffer, cmdBufferSize);

        if (waitForData(id, CHAIN_MONO_SET_SCROLL_TEXT, timeout)) {
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

chain_status_t ChainMono::getMonoStringScroll(uint8_t id, char *string, mono_scroll_dir_t *dir,
                                              mono_scroll_mode_t *mode, uint16_t *IntervalMs, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_MONO_GET_SCROLL_TEXT, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_MONO_GET_SCROLL_TEXT, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *dir           = (mono_scroll_dir_t)((returnPacket[6] >> 4) & 0x0F);
                *mode          = (mono_scroll_mode_t)(returnPacket[6] & 0x0F);
                *IntervalMs    = (uint16_t)(returnPacket[7] | (returnPacket[8] << 8));
                uint8_t length = returnPacket[9];
                for (int i = 0; i < length; i++) {
                    string[i] = returnPacket[10 + i];
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

chain_status_t ChainMono::setMonoStringScrollState(uint8_t id, mono_scroll_state_t state, uint8_t *operationStatus,
                                                   unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)state;
        sendPacket(id, CHAIN_MONO_SET_SCROLL_TEXT_STATE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_MONO_SET_SCROLL_TEXT_STATE, timeout)) {
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

chain_status_t ChainMono::getMonoStringScrollState(uint8_t id, mono_scroll_state_t *state, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_MONO_GET_SCROLL_TEXT_STATE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_MONO_GET_SCROLL_TEXT_STATE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *state = (mono_scroll_state_t)returnPacket[6];
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

chain_status_t ChainMono::setMonoRotation(uint8_t id, mono_rotation_t rotation, uint8_t *operationStatus,
                                          chain_save_flash_t saveToFlash, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)rotation;
        cmdBuffer[cmdBufferSize++] = saveToFlash;
        sendPacket(id, CHAIN_MONO_SET_ROTATION, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_MONO_SET_ROTATION, timeout)) {
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

chain_status_t ChainMono::getMonoRotation(uint8_t id, mono_rotation_t *rotation, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_MONO_GET_ROTATION, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_MONO_GET_ROTATION, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *rotation = (mono_rotation_t)returnPacket[6];
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

chain_status_t ChainMono::setMonoBrightness(uint8_t id, mono_brightness_level_t brightness, uint8_t *operationStatus,
                                            chain_save_flash_t saveToFlash, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = (uint8_t)brightness;
        cmdBuffer[cmdBufferSize++] = saveToFlash;
        sendPacket(id, CHAIN_MONO_SET_BRIGHTNESS, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_MONO_SET_BRIGHTNESS, timeout)) {
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

chain_status_t ChainMono::getMonoBrightness(uint8_t id, mono_brightness_level_t *brightness, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_MONO_GET_BRIGHTNESS, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_MONO_GET_BRIGHTNESS, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *brightness = (mono_brightness_level_t)returnPacket[6];
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

chain_status_t ChainMono::setMonoClear(uint8_t id, uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_MONO_CLEAR, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_MONO_CLEAR, timeout)) {
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

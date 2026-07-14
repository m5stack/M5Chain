/*
 *SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */
#include "Unit8Servos2Chain/Unit8Servos2Chain.hpp"

chain_status_t Unit8Servos2Chain::setServosMode(uint8_t id, uint8_t gpio, user_gpio_mode_t mode,
                                                uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (gpio >= SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = gpio;
        cmdBuffer[cmdBufferSize++] = mode;
        sendPacket(id, SERVOS2_CHAIN_SET_MODE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_SET_MODE, timeout)) {
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

chain_status_t Unit8Servos2Chain::setServosModeAll(uint8_t id, user_gpio_mode_t *mode, uint8_t num,
                                                   uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (mode == nullptr || num != SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        for (uint8_t i = 0; i < num; i++) {
            cmdBuffer[cmdBufferSize++] = mode[i];
        }
        sendPacket(id, SERVOS2_CHAIN_SET_MODE_ALL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_SET_MODE_ALL, timeout)) {
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

chain_status_t Unit8Servos2Chain::getServosMode(uint8_t id, uint8_t gpio, user_gpio_mode_t *mode,
                                                uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (gpio >= SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = gpio;
        sendPacket(id, SERVOS2_CHAIN_GET_MODE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_MODE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
                if (returnPacket[6] == 1) {
                    *mode = (user_gpio_mode_t)returnPacket[7];
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

chain_status_t Unit8Servos2Chain::getServosModeAll(uint8_t id, user_gpio_mode_t *mode, uint8_t num,
                                                   unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (mode == nullptr || num != SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, SERVOS2_CHAIN_GET_MODE_ALL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_MODE_ALL, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                for (uint8_t i = 0; i < num; i++) {
                    mode[i] = (user_gpio_mode_t)returnPacket[6 + i];
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

chain_status_t Unit8Servos2Chain::setServosInputPuPd(uint8_t id, uint8_t gpio, user_gpio_pull_t pu_pd,
                                                     uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (gpio >= SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = gpio;
        cmdBuffer[cmdBufferSize++] = pu_pd;
        sendPacket(id, SERVOS2_CHAIN_SET_INPUT_PU_PD, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_SET_INPUT_PU_PD, timeout)) {
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

chain_status_t Unit8Servos2Chain::setServosInputPuPdAll(uint8_t id, user_gpio_pull_t *pu_pd, uint8_t size,
                                                        uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (pu_pd == nullptr || size != SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        for (uint8_t i = 0; i < size; i++) {
            cmdBuffer[cmdBufferSize++] = pu_pd[i];
        }
        sendPacket(id, SERVOS2_CHAIN_SET_INPUT_PU_PD_ALL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_SET_INPUT_PU_PD_ALL, timeout)) {
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

chain_status_t Unit8Servos2Chain::getServosInputPuPd(uint8_t id, uint8_t gpio, user_gpio_pull_t *pu_pd,
                                                     uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (gpio >= SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = gpio;
        sendPacket(id, SERVOS2_CHAIN_GET_INPUT_PU_PD, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_INPUT_PU_PD, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
                if (returnPacket[6] == 1) {
                    *pu_pd = (user_gpio_pull_t)returnPacket[7];
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

chain_status_t Unit8Servos2Chain::getServosInputPuPdAll(uint8_t id, user_gpio_pull_t *pu_pd, uint8_t size,
                                                        unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (pu_pd == nullptr || size != SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, SERVOS2_CHAIN_GET_INPUT_PU_PD_ALL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_INPUT_PU_PD_ALL, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                for (uint8_t i = 0; i < size; i++) {
                    pu_pd[i] = (user_gpio_pull_t)returnPacket[6 + i];
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

chain_status_t Unit8Servos2Chain::getServosInputLevel(uint8_t id, uint8_t gpio, user_sys_gpio_level_t *level,
                                                      uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (gpio >= SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = gpio;
        sendPacket(id, SERVOS2_CHAIN_GET_INPUT_STATUS, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_INPUT_STATUS, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
                if (returnPacket[6] == 1) {
                    *level = (user_sys_gpio_level_t)returnPacket[7];
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

chain_status_t Unit8Servos2Chain::getServosInputLevelAll(uint8_t id, user_sys_gpio_level_t *level, uint8_t size,
                                                         unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (level == nullptr || size != SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, SERVOS2_CHAIN_GET_INPUT_STATUS_ALL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_INPUT_STATUS_ALL, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                for (uint8_t i = 0; i < size; i++) {
                    level[i] = (user_sys_gpio_level_t)returnPacket[6 + i];
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

chain_status_t Unit8Servos2Chain::setServosOutputLevel(uint8_t id, uint8_t gpio, user_sys_gpio_level_t level,
                                                       uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (gpio >= SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = gpio;
        cmdBuffer[cmdBufferSize++] = level;
        sendPacket(id, SERVOS2_CHAIN_SET_OUTPUT_STATUS, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_SET_OUTPUT_STATUS, timeout)) {
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

chain_status_t Unit8Servos2Chain::setServosOutputLevelAll(uint8_t id, user_sys_gpio_level_t *level, uint8_t size,
                                                          uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (level == nullptr || size != SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        for (uint8_t i = 0; i < size; i++) {
            cmdBuffer[cmdBufferSize++] = level[i];
        }
        sendPacket(id, SERVOS2_CHAIN_SET_OUTPUT_STATUS_ALL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_SET_OUTPUT_STATUS_ALL, timeout)) {
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

chain_status_t Unit8Servos2Chain::getServosOutputLevel(uint8_t id, uint8_t gpio, user_sys_gpio_level_t *level,
                                                       uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (gpio >= SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = gpio;
        sendPacket(id, SERVOS2_CHAIN_GET_OUTPUT_STATUS, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_OUTPUT_STATUS, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
                if (returnPacket[6] == 1) {
                    *level = (user_sys_gpio_level_t)returnPacket[7];
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
chain_status_t Unit8Servos2Chain::getServosOutputLevelAll(uint8_t id, user_sys_gpio_level_t *level, uint8_t size,
                                                          unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (level == nullptr || size != SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, SERVOS2_CHAIN_GET_OUTPUT_STATUS_ALL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_OUTPUT_STATUS_ALL, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                for (uint8_t i = 0; i < size; i++) {
                    level[i] = (user_sys_gpio_level_t)returnPacket[6 + i];
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

chain_status_t Unit8Servos2Chain::getServosAdcValue(uint8_t id, uint8_t gpio, uint16_t *value, uint8_t *operationStatus,
                                                    unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (gpio >= SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = gpio;
        sendPacket(id, SERVOS2_CHAIN_GET_ADC_VALUE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_ADC_VALUE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
                if (returnPacket[6] == 1) {
                    *value = (returnPacket[8] << 8) | returnPacket[7];
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

chain_status_t Unit8Servos2Chain::getServosAdcValueAll(uint8_t id, uint16_t *value, uint8_t size, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (value == nullptr || size != SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, SERVOS2_CHAIN_GET_ADC_VALUE_ALL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_ADC_VALUE_ALL, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                for (uint8_t i = 0; i < size; i++) {
                    value[i] = (returnPacket[7 + i * 2] << 8) | returnPacket[6 + i * 2];
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

chain_status_t Unit8Servos2Chain::setServosAngle(uint8_t id, uint8_t gpio, uint8_t angle, uint8_t *operationStatus,
                                                 unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (gpio >= SERVOS2_GPIO_NUM_MAX || angle > 180) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = gpio;
        cmdBuffer[cmdBufferSize++] = angle;
        sendPacket(id, SERVOS2_CHAIN_SET_SERVO_ANGLE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_SET_SERVO_ANGLE, timeout)) {
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

chain_status_t Unit8Servos2Chain::setServosAngleAll(uint8_t id, uint8_t *angle, uint8_t size, uint8_t *operationStatus,
                                                    unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (angle == nullptr || size != SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        for (uint8_t i = 0; i < size; i++) {
            cmdBuffer[cmdBufferSize++] = angle[i];
        }
        sendPacket(id, SERVOS2_CHAIN_SET_SERVO_ANGLE_ALL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_SET_SERVO_ANGLE_ALL, timeout)) {
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
chain_status_t Unit8Servos2Chain::getServosAngle(uint8_t id, uint8_t gpio, uint8_t *angle, uint8_t *operationStatus,
                                                 unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (gpio >= SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = gpio;
        sendPacket(id, SERVOS2_CHAIN_GET_SERVO_ANGLE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_SERVO_ANGLE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
                if (returnPacket[6] == 1) {
                    *angle = returnPacket[7];
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
chain_status_t Unit8Servos2Chain::getServosAngleAll(uint8_t id, uint8_t *angle, uint8_t size, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (angle == nullptr || size != SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, SERVOS2_CHAIN_GET_SERVO_ANGLE_ALL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_SERVO_ANGLE_ALL, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                for (uint8_t i = 0; i < size; i++) {
                    angle[i] = returnPacket[6 + i];
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

chain_status_t Unit8Servos2Chain::setServosRGBConfig(uint8_t id, uint8_t gpio, uint8_t config, uint8_t *operationStatus,
                                                     unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (gpio >= SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = gpio;
        cmdBuffer[cmdBufferSize++] = config;
        sendPacket(id, SERVOS2_CHAIN_SET_RGB_CONFIG, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_SET_RGB_CONFIG, timeout)) {
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

chain_status_t Unit8Servos2Chain::setServosRGBConfigAll(uint8_t id, uint8_t *config, uint8_t size,
                                                        uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (config == nullptr || size != SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        for (uint8_t i = 0; i < size; i++) {
            cmdBuffer[cmdBufferSize++] = config[i];
        }
        sendPacket(id, SERVOS2_CHAIN_SET_RGB_CONFIG_ALL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_SET_RGB_CONFIG_ALL, timeout)) {
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
chain_status_t Unit8Servos2Chain::getServosRGBConfig(uint8_t id, uint8_t gpio, uint8_t *config,
                                                     uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (gpio >= SERVOS2_GPIO_NUM_MAX || config == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = gpio;
        sendPacket(id, SERVOS2_CHAIN_GET_RGB_CONFIG, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_RGB_CONFIG, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
                if (returnPacket[6] == 1) {
                    *config = returnPacket[7];
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

chain_status_t Unit8Servos2Chain::getServosRGBConfigAll(uint8_t id, uint8_t *config, uint8_t size,
                                                        unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (config == nullptr || size != SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, SERVOS2_CHAIN_GET_RGB_CONFIG_ALL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_RGB_CONFIG_ALL, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                for (uint8_t i = 0; i < size; i++) {
                    config[i] = returnPacket[6 + i];
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

chain_status_t Unit8Servos2Chain::setServosRGBBuffer(uint8_t id, uint8_t index, uint32_t buffer,
                                                     uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (index >= RGB_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = index;
        uint8_t rgb[3];
        rgb[0] = buffer >> 16;
        rgb[1] = buffer >> 8;
        rgb[2] = buffer;
        for (uint8_t i = 0; i < 3; i++) {
            cmdBuffer[cmdBufferSize++] = rgb[i];
        }
        sendPacket(id, SERVOS2_CHAIN_SET_RGB_BUFFER, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_SET_RGB_BUFFER, timeout)) {
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
chain_status_t Unit8Servos2Chain::setServosRGBBufferAll(uint8_t id, uint32_t *buffer, uint8_t size,
                                                        unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (buffer == nullptr || size != RGB_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }
    if (acquireMutex()) {
        cmdBufferSize = 0;
        for (uint8_t i = 0; i < size; i++) {
            uint8_t rgb[3];
            rgb[0] = buffer[i] >> 16;
            rgb[1] = buffer[i] >> 8;
            rgb[2] = buffer[i];
            for (uint8_t j = 0; j < 3; j++) {
                cmdBuffer[cmdBufferSize++] = rgb[j];
            }
        }
        sendPacket(id, SERVOS2_CHAIN_SET_RGB_BUFFER_ALL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_SET_RGB_BUFFER_ALL, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
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

chain_status_t Unit8Servos2Chain::getServosRGBBuffer(uint8_t id, uint8_t index, uint32_t *buffer,
                                                     uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (buffer == nullptr || index >= RGB_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = index;
        sendPacket(id, SERVOS2_CHAIN_GET_RGB_BUFFER, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_RGB_BUFFER, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
                if (returnPacket[6] == 1) {
                    *buffer = returnPacket[7] << 16 | returnPacket[8] << 8 | returnPacket[9];
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

chain_status_t Unit8Servos2Chain::getServosRGBBufferAll(uint8_t id, uint32_t *buffer, uint8_t size,
                                                        unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (buffer == nullptr || size != RGB_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, SERVOS2_CHAIN_GET_RGB_BUFFER_ALL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_RGB_BUFFER_ALL, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                for (uint8_t i = 0; i < size; i++) {
                    buffer[i] = returnPacket[6 + i * 3] << 16 | returnPacket[7 + i * 3] << 8 | returnPacket[8 + i * 3];
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

chain_status_t Unit8Servos2Chain::setServosPwmDuty(uint8_t id, uint8_t gpio, uint8_t duty, uint8_t *operationStatus,
                                                   unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (gpio >= SERVOS2_GPIO_NUM_MAX || duty > 100 || operationStatus == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = gpio;
        cmdBuffer[cmdBufferSize++] = duty;
        sendPacket(id, SERVOS2_CHAIN_SET_PWM_DUTY, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_SET_PWM_DUTY, timeout)) {
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

chain_status_t Unit8Servos2Chain::setServosPwmDutyAll(uint8_t id, uint8_t *duty, uint8_t size, uint8_t *operationStatus,
                                                      unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (duty == nullptr || size != SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        for (uint8_t i = 0; i < size; i++) {
            cmdBuffer[cmdBufferSize++] = duty[i];
        }
        sendPacket(id, SERVOS2_CHAIN_SET_PWM_DUTY_ALL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_SET_PWM_DUTY_ALL, timeout)) {
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
chain_status_t Unit8Servos2Chain::getServosPwmDuty(uint8_t id, uint8_t gpio, uint8_t *duty, uint8_t *operationStatus,
                                                   unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (gpio >= SERVOS2_GPIO_NUM_MAX || duty == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = gpio;
        sendPacket(id, SERVOS2_CHAIN_GET_PWM_DUTY, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_PWM_DUTY, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
                if (returnPacket[6] == 1) {
                    *duty = returnPacket[7];
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
chain_status_t Unit8Servos2Chain::getServosPwmDutyAll(uint8_t id, uint8_t *duty, uint8_t size, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (duty == nullptr || size != SERVOS2_GPIO_NUM_MAX) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, SERVOS2_CHAIN_GET_PWM_DUTY_ALL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_PWM_DUTY_ALL, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                for (uint8_t i = 0; i < size; i++) {
                    duty[i] = returnPacket[6 + i];
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

chain_status_t Unit8Servos2Chain::setServosTimeConfig(uint8_t id, uint8_t time, uint16_t freq, uint8_t *operationStatus,
                                                      unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (time > 1 || freq == 0) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = time;
        cmdBuffer[cmdBufferSize++] = freq & 0xFF;
        cmdBuffer[cmdBufferSize++] = (freq >> 8) & 0xFF;
        sendPacket(id, SERVOS2_CHAIN_SET_TIME_CONFIG, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_SET_TIME_CONFIG, timeout)) {
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
chain_status_t Unit8Servos2Chain::setServosTimeConfigAll(uint8_t id, uint16_t *freq, uint8_t size,
                                                         uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (freq == nullptr || size != 2) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = freq[0] & 0xFF;
        cmdBuffer[cmdBufferSize++] = (freq[0] >> 8) & 0xFF;
        cmdBuffer[cmdBufferSize++] = freq[1] & 0xFF;
        cmdBuffer[cmdBufferSize++] = (freq[1] >> 8) & 0xFF;
        sendPacket(id, SERVOS2_CHAIN_SET_TIME_CONFIG_ALL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_SET_TIME_CONFIG_ALL, timeout)) {
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
chain_status_t Unit8Servos2Chain::getServosTimeConfig(uint8_t id, uint8_t time, uint16_t *frequency,
                                                      uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (time > 1 || frequency == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = time;
        sendPacket(id, SERVOS2_CHAIN_GET_TIME_CONFIG, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_TIME_CONFIG, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
                if (returnPacket[6] == 1) {
                    *frequency = (returnPacket[8] << 8) | returnPacket[7];
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

chain_status_t Unit8Servos2Chain::getServosTimeConfigAll(uint8_t id, uint16_t *frequency, uint8_t size,
                                                         unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (frequency == nullptr || size != 2) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, SERVOS2_CHAIN_GET_TIME_CONFIG_ALL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_TIME_CONFIG_ALL, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                for (uint8_t i = 0; i < size; i++) {
                    frequency[i] = (returnPacket[7 + i * 2] << 8) | returnPacket[6 + i * 2];
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

chain_status_t Unit8Servos2Chain::getServosVref(uint8_t id, uint16_t *vref, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (vref == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, SERVOS2_CHAIN_GET_VREF, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_VREF, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *vref = (returnPacket[7] << 8) | returnPacket[6];
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

chain_status_t Unit8Servos2Chain::getServosGroveVoltage(uint8_t id, uint16_t *voltage, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (voltage == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, SERVOS2_CHAIN_GET_GROVE_VOLTAGE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_GROVE_VOLTAGE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *voltage = (returnPacket[7] << 8) | returnPacket[6];
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
chain_status_t Unit8Servos2Chain::getServosDcVoltage(uint8_t id, uint16_t *voltage, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (voltage == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, SERVOS2_CHAIN_GET_DC_VOLTAGE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_DC_VOLTAGE, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *voltage = (returnPacket[7] << 8) | returnPacket[6];
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
chain_status_t Unit8Servos2Chain::getServosSysCurrent(uint8_t id, uint16_t *current, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;
    if (current == nullptr) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, SERVOS2_CHAIN_GET_SYS_CURRENT, cmdBuffer, cmdBufferSize);
        if (waitForData(id, SERVOS2_CHAIN_GET_SYS_CURRENT, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *current = (returnPacket[7] << 8) | returnPacket[6];
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

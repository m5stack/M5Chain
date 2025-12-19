/*
 *SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#include "UnitChainBus.hpp"

chain_status_t UnitChainBus::setChainBusI2cMode(uint8_t id, i2c_speed_t i2cSpeed, uint8_t *operationStatus,
                                                unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (i2cSpeed != CHAIN_I2C_LOW_SPEED_100KHZ && i2cSpeed != CHAIN_I2C_HIGH_SPEED_400KHZ) {
        return CHAIN_PARAMETER_ERROR;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = i2cSpeed;
        sendPacket(id, CHAIN_I2C_INIT, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_I2C_INIT, timeout)) {
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

chain_status_t UnitChainBus::chainBusI2cRead(uint8_t id, uint8_t i2cAddr, uint8_t readLength, uint8_t *buffer,
                                             uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = i2cAddr;
        cmdBuffer[cmdBufferSize++] = readLength;
        sendPacket(id, CHAIN_I2C_READ, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_I2C_READ, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
                if (returnPacket[6] == CHAIN_UART_OPERATION_SUCCESS) {
                    for (uint8_t i = 0; i < readLength; i++) {
                        buffer[i] = returnPacket[7 + i];
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

chain_status_t UnitChainBus::chainBusI2cWrite(uint8_t id, uint8_t i2cAddr, uint8_t writeLength, uint8_t *buffer,
                                              uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (writeLength > I2C_READ_MAX_SIZE || i2cAddr > I2C_WRITE_MAX_SIZE) {
        status = CHAIN_PARAMETER_ERROR;
        return status;
    }

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = i2cAddr;
        cmdBuffer[cmdBufferSize++] = writeLength;
        for (uint8_t i = 0; i < writeLength; i++) {
            cmdBuffer[cmdBufferSize++] = buffer[i];
        }
        sendPacket(id, CHAIN_I2C_WRITE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_I2C_WRITE, timeout)) {
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
chain_status_t UnitChainBus::chainBusI2cMemRead(uint8_t id, uint8_t i2cAddr, uint16_t regAddr, i2c_reg_len_t regLength,
                                                uint8_t readLength, uint8_t *buffer, uint8_t *operationStatus,
                                                unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = i2cAddr;
        cmdBuffer[cmdBufferSize++] = regLength;
        cmdBuffer[cmdBufferSize++] = regAddr & 0xFF;
        cmdBuffer[cmdBufferSize++] = (regAddr >> 8) & 0xFF;
        cmdBuffer[cmdBufferSize++] = readLength;
        sendPacket(id, CHAIN_I2C_MEM_READ, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_I2C_MEM_READ, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
                if (*operationStatus == CHAIN_UART_OPERATION_SUCCESS) {
                    for (uint8_t i = 0; i < readLength; i++) {
                        buffer[i] = returnPacket[7 + i];
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

chain_status_t UnitChainBus::chainBusI2cMemWrite(uint8_t id, uint8_t i2cAddr, uint16_t regAddr, i2c_reg_len_t regLength,
                                                 uint8_t writeLength, uint8_t *buffer, uint8_t *operationStatus,
                                                 unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = i2cAddr;
        cmdBuffer[cmdBufferSize++] = regLength;
        cmdBuffer[cmdBufferSize++] = regAddr & 0xFF;
        cmdBuffer[cmdBufferSize++] = (regAddr >> 8) & 0xFF;
        cmdBuffer[cmdBufferSize++] = writeLength;
        for (uint8_t i = 0; i < writeLength; i++) {
            cmdBuffer[cmdBufferSize++] = buffer[i];
        }
        sendPacket(id, CHAIN_I2C_MEM_WRITE, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_I2C_MEM_WRITE, timeout)) {
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

chain_status_t UnitChainBus::getChainBusI2cScanAddr(uint8_t id, uint8_t *i2cAddrNums, uint8_t *buffer, uint8_t size,
                                                    uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_I2C_SCAN_ADDR, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_I2C_SCAN_ADDR, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
                if (*operationStatus == CHAIN_UART_OPERATION_SUCCESS) {
                    *i2cAddrNums = returnPacket[7];
                    if (returnPacket[7] > size) {
                        status = CHAIN_PARAMETER_ERROR;
                    } else {
                        for (uint8_t i = 0; i < returnPacket[7]; i++) {
                            buffer[i] = returnPacket[8 + i];
                        }
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

chain_status_t UnitChainBus::setChainBusOutputMode(uint8_t id, gpio_pin_t gpio, gpio_output_t gpioOutputMode,
                                                   gpio_pull_t gpioPull, uint8_t *operationStatus,
                                                   unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = gpio;
        cmdBuffer[cmdBufferSize++] = gpioOutputMode;
        cmdBuffer[cmdBufferSize++] = gpioPull;
        sendPacket(id, CHAIN_GPIO_OUTPUT_INIT, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_GPIO_OUTPUT_INIT, timeout)) {
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

chain_status_t UnitChainBus::setChainBusOutputLevel(uint8_t id, gpio_pin_t gpio, gpio_level_t gpioLevel,
                                                    uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = gpio;
        cmdBuffer[cmdBufferSize++] = gpioLevel;
        sendPacket(id, CHAIN_GPIO_SET_OUTPUT_LEVEL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_GPIO_SET_OUTPUT_LEVEL, timeout)) {
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
chain_status_t UnitChainBus::getChainBusOutputLevel(uint8_t id, gpio_pin_t gpio, gpio_level_t *gpioLevel,
                                                    uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = gpio;
        sendPacket(id, CHAIN_GPIO_READ_GPIO_LEVEL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_GPIO_READ_GPIO_LEVEL, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
                if (*operationStatus == CHAIN_UART_OPERATION_SUCCESS) {
                    *gpioLevel = (gpio_level_t)returnPacket[7];
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

chain_status_t UnitChainBus::setChainBusInputMode(uint8_t id, gpio_pin_t gpio, gpio_pull_t gpioPull,
                                                  uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = gpio;
        cmdBuffer[cmdBufferSize++] = gpioPull;
        sendPacket(id, CHAIN_GPIO_INPUT_INIT, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_GPIO_INPUT_INIT, timeout)) {
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

chain_status_t UnitChainBus::getChainBusInputLevel(uint8_t id, gpio_pin_t gpio, uint8_t *gpioLevel,
                                                   uint8_t *operationStatus, unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = gpio;
        sendPacket(id, CHAIN_GPIO_READ_GPIO_LEVEL, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_GPIO_READ_GPIO_LEVEL, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
                if (*operationStatus == CHAIN_UART_OPERATION_SUCCESS) {
                    *gpioLevel = returnPacket[7];
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

chain_status_t UnitChainBus::setChainBusNvicMode(uint8_t id, gpio_pin_t gpio, gpio_pull_t gpioPull,
                                                 nvic_trigger_t triggerMode, uint8_t *operationStatus,
                                                 unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = gpio;
        cmdBuffer[cmdBufferSize++] = gpioPull;
        cmdBuffer[cmdBufferSize++] = triggerMode;
        sendPacket(id, CHAIN_GPIO_EXTERNAL_NVIC_INIT, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_GPIO_EXTERNAL_NVIC_INIT, timeout)) {
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

bool UnitChainBus::getChainBusNvicTriggerStatus(uint8_t id, uint16_t *status)
{
    processIncomingData();
    bool findStatus = 0;
    record_info_t result;
    findStatus = findRecord(&recordList, id, &result);
    if (findStatus == true) {
        *status = result.type;
        return true;
    }
    return false;
}

chain_status_t UnitChainBus::setChainBusAdcMode(uint8_t id, gpio_pin_t gpio, uint8_t *operationStatus,
                                                unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = gpio;
        sendPacket(id, CHAIN_GPIO_ADC_INIT, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_GPIO_ADC_INIT, timeout)) {
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

chain_status_t UnitChainBus::getChainBusAdcValue(uint8_t id, gpio_pin_t gpio, uint16_t *value, uint8_t *operationStatus,
                                                 unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize              = 0;
        cmdBuffer[cmdBufferSize++] = gpio;
        sendPacket(id, CHAIN_GPIO_ADC_READ, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_GPIO_ADC_READ, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *operationStatus = returnPacket[6];
                if (*operationStatus == CHAIN_UART_OPERATION_SUCCESS) {
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

chain_status_t UnitChainBus::getChainBusWorkMode(uint8_t id, work_status_t *gpio1, work_status_t *gpio2,
                                                 unsigned long timeout)
{
    chain_status_t status = CHAIN_OK;

    if (acquireMutex()) {
        cmdBufferSize = 0;
        sendPacket(id, CHAIN_GET_WORK_STATION, cmdBuffer, cmdBufferSize);
        if (waitForData(id, CHAIN_GET_WORK_STATION, timeout)) {
            if (checkPacket(returnPacket, returnPacketSize)) {
                *gpio1 = (work_status_t)returnPacket[6];
                *gpio2 = (work_status_t)returnPacket[7];
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
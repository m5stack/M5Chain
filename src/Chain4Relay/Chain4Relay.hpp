/*
 *SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#ifndef CHAIN_4RELAY_HPP_
#define CHAIN_4RELAY_HPP_

#include "ChainCommon/ChainCommon.hpp"

static constexpr uint8_t CHAIN_4RELAY_CHANNEL_COUNT = 4;

typedef enum {
    CHAIN_4RELAY_SET_LEVEL = 0x30,
    CHAIN_4RELAY_GET_LEVEL = 0x31,
} CHAIN_4RELAY_CMD_T;

class Chain4Relay : virtual public ChainCommon {
public:
    /**
     * @brief Set one active-high relay output using command 0x30.
     * @param id Device position in the chain, starting from 1.
     * @param index Relay index (0..3), corresponding to CH1..CH4.
     * @param level Output level: 0 = low/off, 1 = high/on.
     * @param operationStatus Receives the STM32 result (1: success, 0: failure).
     * @param timeout Response timeout in milliseconds.
     * @return Communication status; CHAIN_OK does not imply operation success.
     */
    chain_status_t set4RelayLevel(uint8_t id, uint8_t index, uint8_t level, uint8_t *operationStatus,
                                  unsigned long timeout = 100);

    /**
     * @brief Set all four relay outputs with one command 0x30 request byte.
     * @param id Device position in the chain, starting from 1.
     * @param levels Relay levels packed in bits 0..3 for relay 0..3.
     * @param operationStatus Receives the STM32 result (1: success, 0: failure).
     * @param timeout Response timeout in milliseconds.
     * @return Communication status; CHAIN_OK does not imply operation success.
     */
    chain_status_t set4RelayLevels(uint8_t id, uint8_t levels, uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Get the current four relay output levels.
     * @param id Device position in the chain, starting from 1.
     * @param levels Receives relay levels in bits 0..3 for relay 0..3.
     * @param timeout Response timeout in milliseconds.
     * @return Communication status.
     */
    chain_status_t get4RelayLevels(uint8_t id, uint8_t *levels, unsigned long timeout = 100);

    uint16_t get4RelayTypeCode(void);

private:
    chain_status_t send4RelayRequest(uint8_t id, uint8_t request, uint8_t *operationStatus, unsigned long timeout);
};

#endif  // CHAIN_4RELAY_HPP_

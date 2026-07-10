/*
 *SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#ifndef CHAIN_PIR_HPP_
#define CHAIN_PIR_HPP_

#include "ChainCommon/ChainCommon.hpp"

typedef enum {
    CHAIN_PIR_NO_PERSON = 0x00, /**< Status indicating the PIR is not detecting any person. */
    CHAIN_PIR_PERSON    = 0x01, /**< Status indicating the PIR is detecting a person. */
} pir_detect_result_t;          /**< Enumeration for recording the PIR's current detect result of the pir device. */

typedef enum {
    CHAIN_PIR_REPORT_PERSON_LEAVE = 0x0500, /**< Report indicating a person has left the detection area. */
    CHAIN_PIR_REPORT_PERSON_COME  = 0x0501, /**< Report indicating a person has entered the detection area. */
} pir_detect_report_t; /**< Enumeration for recording the PIR's current detect result of the pir device at Report Mode.
                        */

/**
 * @brief Chain PIR detect mode enumeration.
 */
typedef enum {
    CHAIN_DETECT_NONE_REPORT_MODE = 0x00, /**< None report mode. */
    CHAIN_DETECT_REPORT_MODE      = 0x01, /**< Report mode. */
} chain_detect_mode_t;

/**
 * @brief Enumeration for CHAIN_PIR device commands.
 *
 * This enumeration defines command codes for various operations of the CHAIN_PIR device.
 */
typedef enum {
    CHAIN_PIR_GET_IR_STATUS           = 0x37, /**< Command to get the current IR induction status */
    CHAIN_PIR_SET_AUTO_SEND_IR_STATUS = 0xE1, /**< Command to set the auto-send ir status */
    CHAIN_PIR_GET_AUTO_SEND_IR_STATUS = 0xE2, /**< Command to get the auto-send ir status */
    CHAIN_SET_TRIGGER_KEEP_SECONDS    = 0xE3, /**< Command to set keep seconds for person-come trigger reports */
    CHAIN_GET_TRIGGER_KEEP_SECONDS    = 0xE4, /**< Command to get keep seconds for person-come trigger reports */
} CHAIN_PIR_CMD_T;                            /**< Command types for Chain_PIR device operations */

class ChainPIR : virtual public ChainCommon {
public:
    /**
     * @brief Retrieves the current IR detection status of the PIR device at the specified position in the chain.
     *
     * This function gets whether the PIR device is currently detecting a person.
     *
     * @param id The position of the PIR device in the chain (starting from 1).
     * @param ir_status Pointer to store the current detection status (`CHAIN_PIR_PERSON` or `CHAIN_PIR_NO_PERSON`).
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return The operation status (e.g., `CHAIN_OK`, `CHAIN_TIMEOUT`, etc.).
     */
    chain_status_t getIRStatus(uint8_t id, pir_detect_result_t* ir_status, unsigned long timeout = 100);

    /**
     * @brief Sets the PIR detect trigger mode for the PIR device at the specified position in the chain.
     *
     * This function sets the PIR detect trigger mode for the PIR device. The mode can either be
     * 'CHAIN_DETECT_NONE_REPORT_MODE' or 'CHAIN_DETECT_REPORT_MODE'.
     *
     * @param id The position of the PIR device in the chain (starting from 1).
     * @param auto_status The desired trigger mode (CHAIN_DETECT_REPORT_MODE or CHAIN_DETECT_NONE_REPORT_MODE).
     * @param operationStatus Pointer to store the operation status.
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setPIRDetectTriggerMode(uint8_t id, chain_detect_mode_t auto_status, uint8_t* operationStatus,
                                           unsigned long timeout = 100);

    /**
     * @brief Gets the PIR detect trigger mode for the PIR device at the specified position in the chain.
     *
     * This function retrieves the current PIR detect trigger mode of the PIR device.
     *
     * @param id The position of the PIR device in the chain (starting from 1).
     * @param auto_status Pointer to store the current trigger mode (CHAIN_DETECT_REPORT_MODE or
     * CHAIN_DETECT_NONE_REPORT_MODE).
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t getPIRDetectTriggerMode(uint8_t id, chain_detect_mode_t* auto_status, unsigned long timeout = 100);

    /**
     * @brief Gets the PIR detect report status.
     *
     * This function retrieves the latest PIR report status, indicating whether a person has entered or left the
     * detection area.
     *
     * @param id The position of the PIR device in the chain (starting from 1).
     * @param triggerStatus Pointer to store the PIR detect report status (`CHAIN_PIR_REPORT_PERSON_LEAVE` or
     * `CHAIN_PIR_REPORT_PERSON_COME`).
     *
     * @return Returns true if a matching trigger report was found, otherwise returns false.
     */
    bool getPIRDetectTrigger(uint8_t id, pir_detect_report_t* triggerStatus);

    /**
     * @brief Sets keep seconds for the PIR person-come trigger and IR status update.
     *
     * This function sets how long the PIR device keeps the person-come trigger and corresponding IR status value. If a
     * person leaves during this keep time, the leave event will not be reported.
     *
     * @param id The position of the PIR device in the chain (starting from 1).
     * @param keepSeconds Seconds to keep the person-come trigger status and IR status value, limited to 0 to 255
     * seconds.
     * @param operationStatus Pointer to store the operation status.
     * @param saveToFlash Optional parameter to specify whether to save the setting to flash memory (default is 0, no
     * save).
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t setPIRComeTriggerKeepSeconds(uint8_t id, uint8_t keepSeconds, uint8_t* operationStatus,
                                                uint8_t saveToFlash = 0, unsigned long timeout = 100);

    /**
     * @brief Gets keep seconds for the PIR person-come trigger and IR status update.
     *
     * This function retrieves how long the PIR device keeps the person-come trigger and corresponding IR status value.
     * If a person leaves during this keep time, the leave event will not be reported.
     *
     * @param id The position of the PIR device in the chain (starting from 1).
     * @param keepSeconds Pointer to store the keep seconds.
     * @param timeout The timeout duration for the operation in milliseconds (default is 100ms).
     *
     * @return Operation status (e.g., CHAIN_OK, CHAIN_BUSY, etc.).
     */
    chain_status_t getPIRComeTriggerKeepSeconds(uint8_t id, uint8_t* keepSeconds, unsigned long timeout = 100);

private:
};

#endif  // CHAIN_PIR_HPP_

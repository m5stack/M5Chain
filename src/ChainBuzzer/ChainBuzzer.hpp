/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef _CHAIN_BUZZER_HPP_
#define _CHAIN_BUZZER_HPP_

#include <ChainCommon/ChainCommon.hpp>

/**
 * @brief Minimum buzzer frequency in Hz.
 */
#define BUZZER_FREQ_MIN (100)

/**
 * @brief Maximum buzzer frequency in Hz.
 */
#define BUZZER_FREQ_MAX (10000)

/**
 * @brief Maximum buzzer duty cycle in percent.
 */
#define BUZZER_DUTY_MAX (100)

/**
 * @brief Total number of musical notes defined in the note frequency table.
 */
#define NOTE_COUNT (62)

/**
 * @brief Enumeration for buzzer modes.
 */
typedef enum {
    BUZZER_MODE_AUTO_PLAY   = 0x00, /**< Buzzer plays automatically for a set duration. */
    BUZZER_MODE_MANUAL_PLAY = 0x01, /**< Buzzer requires manual start/stop control. */
    BUZZER_MODE_NOTE_PLAY   = 0x02, /**< Buzzer plays musical notes for a set duration. */
} buzzer_mode_t;

/**
 * @brief Enumeration for buzzer on/off status.
 */
typedef enum {
    BUZZER_STATUS_OFF = 0x00, /**< Buzzer output disabled. */
    BUZZER_STATUS_ON  = 0x01, /**< Buzzer output enabled. */
} buzzer_status_t;

/**
 * @brief Command types for Chain_Buzzer device control.
 */
typedef enum {
    CHAIN_BUZZER_SET_MODE      = 0x30, /**< Set buzzer mode (auto/manual). */
    CHAIN_BUZZER_GET_MODE      = 0x31, /**< Get buzzer mode. */
    CHAIN_BUZZER_SET_AUTO_PLAY = 0x32, /**< Start auto-play mode with given frequency/duty/duration. */
    CHAIN_BUZZER_SET_FREQ      = 0x33, /**< Set buzzer frequency. */
    CHAIN_BUZZER_GET_FREQ      = 0x34, /**< Get buzzer frequency. */
    CHAIN_BUZZER_SET_DUTY      = 0x35, /**< Set buzzer duty cycle. */
    CHAIN_BUZZER_GET_DUTY      = 0x36, /**< Get buzzer duty cycle. */
    CHAIN_BUZZER_SET_STATUS    = 0x37, /**< Set buzzer on/off status. */
    CHAIN_BUZZER_GET_STATUS    = 0x38, /**< Get buzzer on/off status. */
    CHAIN_BUZZER_SET_NOTE_PLAY = 0x39, /**< Start note-play mode with given note/duration. */
} chain_buzzer_cmd_t;

/**
 * @brief Note index enumeration for musical note lookup.
 *
 * Defines indices for all supported musical notes from C3 to C8,
 * plus a special REST note for silence. Indices correspond to the
 * note frequency table.
 */
typedef enum {
    // Special note
    NOTE_REST = 0,  // Rest

    // Octave 3 (1 - 12)
    NOTE_C3  = 1,
    NOTE_CS3 = 2,
    NOTE_D3  = 3,
    NOTE_DS3 = 4,
    NOTE_E3  = 5,
    NOTE_F3  = 6,
    NOTE_FS3 = 7,
    NOTE_G3  = 8,
    NOTE_GS3 = 9,
    NOTE_A3  = 10,
    NOTE_AS3 = 11,
    NOTE_B3  = 12,

    // Octave 4 (13 - 24)
    NOTE_C4  = 13,
    NOTE_CS4 = 14,
    NOTE_D4  = 15,
    NOTE_DS4 = 16,
    NOTE_E4  = 17,
    NOTE_F4  = 18,
    NOTE_FS4 = 19,
    NOTE_G4  = 20,
    NOTE_GS4 = 21,
    NOTE_A4  = 22,
    NOTE_AS4 = 23,
    NOTE_B4  = 24,

    // Octave 5 (25 - 36)
    NOTE_C5  = 25,
    NOTE_CS5 = 26,
    NOTE_D5  = 27,
    NOTE_DS5 = 28,
    NOTE_E5  = 29,
    NOTE_F5  = 30,
    NOTE_FS5 = 31,
    NOTE_G5  = 32,
    NOTE_GS5 = 33,
    NOTE_A5  = 34,
    NOTE_AS5 = 35,
    NOTE_B5  = 36,

    // Octave 6 (37 - 48)
    NOTE_C6  = 37,
    NOTE_CS6 = 38,
    NOTE_D6  = 39,
    NOTE_DS6 = 40,
    NOTE_E6  = 41,
    NOTE_F6  = 42,
    NOTE_FS6 = 43,
    NOTE_G6  = 44,
    NOTE_GS6 = 45,
    NOTE_A6  = 46,
    NOTE_AS6 = 47,
    NOTE_B6  = 48,

    // Octave 7 (49 - 60)
    NOTE_C7  = 49,
    NOTE_CS7 = 50,
    NOTE_D7  = 51,
    NOTE_DS7 = 52,
    NOTE_E7  = 53,
    NOTE_F7  = 54,
    NOTE_FS7 = 55,
    NOTE_G7  = 56,
    NOTE_GS7 = 57,
    NOTE_A7  = 58,
    NOTE_AS7 = 59,
    NOTE_B7  = 60,

    // Octave 8 (61)
    NOTE_C8 = 61,
} note_index_t;

/**
 * @brief Musical note name strings corresponding to note indices.
 *
 * Array of human-readable note names that map to the note_index_t enumeration.
 * Used for debugging and display purposes.
 */
static const char *note_names[NOTE_COUNT] = {
    // Special note
    "REST",

    // Octave 3
    "C3", "CS3", "D3", "DS3", "E3", "F3", "FS3", "G3", "GS3", "A3", "AS3", "B3",
    // Octave 4
    "C4", "CS4", "D4", "DS4", "E4", "F4", "FS4", "G4", "GS4", "A4", "AS4", "B4",
    // Octave 5
    "C5", "CS5", "D5", "DS5", "E5", "F5", "FS5", "G5", "GS5", "A5", "AS5", "B5",
    // Octave 6
    "C6", "CS6", "D6", "DS6", "E6", "F6", "FS6", "G6", "GS6", "A6", "AS6", "B6",
    // Octave 7
    "C7", "CS7", "D7", "DS7", "E7", "F7", "FS7", "G7", "GS7", "A7", "AS7", "B7",
    // Octave 8
    "C8"};

/**
 * @brief Class to control a ChainBuzzer device in a multi-device chain.
 */
class ChainBuzzer : virtual public ChainCommon {
public:
    /**
     * @brief Set the buzzer operating mode.
     *
     * @param id The position of the buzzer in the chain (starting from 1).
     * @param mode Desired buzzer mode (auto-play or manual-play).
     * @param operationStatus Pointer to store the operation result (1: success, 0: failure).
     * @param timeout Operation timeout in milliseconds (default 100 ms).
     * @return Operation status code (e.g., CHAIN_OK, CHAIN_PARAMETER_ERROR).
     */
    chain_status_t setBuzzerMode(uint8_t id, buzzer_mode_t mode, uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Get the buzzer operating mode.
     *
     * @param id The position of the buzzer in the chain (starting from 1).
     * @param mode Pointer to store the current buzzer mode.
     * @param timeout Operation timeout in milliseconds (default 100 ms).
     * @return Operation status code (e.g., CHAIN_OK, CHAIN_PARAMETER_ERROR).
     */
    chain_status_t getBuzzerMode(uint8_t id, buzzer_mode_t *mode, unsigned long timeout = 100);

    /**
     * @brief Start buzzer auto-play mode.
     *
     * Automatically plays a tone at the specified frequency and duty for a set duration.
     *
     * @param id The position of the buzzer in the chain (starting from 1).
     * @param freq Buzzer frequency in Hz (range: BUZZER_FREQ_MIN - BUZZER_FREQ_MAX).
     * @param duty Duty cycle in percent (range: 0 - BUZZER_DUTY_MAX).
     * @param duration Duration in milliseconds.
     * @param operationStatus Pointer to store the operation result (1: success, 0: failure，2: mode error).
     * @param timeout Operation timeout in milliseconds (default 100 ms).
     * @return Operation status code (e.g., CHAIN_OK, CHAIN_PARAMETER_ERROR).
     */
    chain_status_t setBuzzerAutoPlay(uint8_t id, uint16_t freq, uint8_t duty, uint16_t duration,
                                     uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Set the buzzer frequency.
     *
     * @param id The position of the buzzer in the chain (starting from 1).
     * @param freq Frequency value in Hz (range: BUZZER_FREQ_MIN - BUZZER_FREQ_MAX).
     * @param operationStatus Pointer to store the operation result.
     * @param timeout Operation timeout in milliseconds.
     * @return Operation status code.
     */
    chain_status_t setBuzzerFreq(uint8_t id, uint16_t freq, uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Get the current buzzer frequency.
     *
     * @param id The position of the buzzer in the chain (starting from 1).
     * @param freq Pointer to store the retrieved frequency in Hz.
     * @param timeout Operation timeout in milliseconds.
     * @return Operation status code (e.g., CHAIN_OK, CHAIN_PARAMETER_ERROR).
     */
    chain_status_t getBuzzerFreq(uint8_t id, uint16_t *freq, unsigned long timeout = 100);

    /**
     * @brief Set the buzzer duty cycle.
     *
     * @param id The position of the buzzer in the chain (starting from 1).
     * @param duty Duty cycle in percent (range: 0 - BUZZER_DUTY_MAX).
     * @param operationStatus Pointer to store the operation result.
     * @param timeout Operation timeout in milliseconds.
     * @return Operation status code.
     */
    chain_status_t setBuzzerDuty(uint8_t id, uint8_t duty, uint8_t *operationStatus, unsigned long timeout = 100);

    /**
     * @brief Get the buzzer duty cycle.
     *
     * @param id The position of the buzzer in the chain (starting from 1).
     * @param duty Pointer to store the retrieved duty cycle in percent.
     * @param timeout Operation timeout in milliseconds.
     * @return Operation status code (e.g., CHAIN_OK, CHAIN_PARAMETER_ERROR).
     */
    chain_status_t getBuzzerDuty(uint8_t id, uint8_t *duty, unsigned long timeout = 100);

    /**
     * @brief Set the buzzer ON/OFF status.
     *
     * When in manual-play mode, this directly controls buzzer output.
     *
     * @param id The position of the buzzer in the chain (starting from 1).
     * @param buzzerStatus Desired buzzer status (BUZZER_STATUS_ON / BUZZER_STATUS_OFF).
     * @param operationStatus Pointer to store the operation result.
     * @param timeout Operation timeout in milliseconds.
     * @return Operation status code (e.g., CHAIN_OK, CHAIN_PARAMETER_ERROR).
     */
    chain_status_t setBuzzerStatus(uint8_t id, buzzer_status_t buzzerStatus, uint8_t *operationStatus,
                                   unsigned long timeout = 100);

    /**
     * @brief Get the buzzer ON/OFF status.
     *
     * @param id The position of the buzzer in the chain (starting from 1).
     * @param buzzerStatus Pointer to store the current buzzer status.
     * @param timeout Operation timeout in milliseconds.
     * @return Operation status code (e.g., CHAIN_OK, CHAIN_PARAMETER_ERROR).
     */
    chain_status_t getBuzzerStatus(uint8_t id, buzzer_status_t *buzzerStatus, unsigned long timeout = 100);

    /**
     * @brief Start buzzer note-play mode.
     *
     * Plays a musical note for a set duration based on the note index.
     *
     * @param id The position of the buzzer in the chain (starting from 1).
     * @param note Musical note index from note_index_t enumeration (e.g., NOTE_C4).
     * @param duration Duration in
     * milliseconds.
     * @param operationStatus Pointer to store the operation result (1: success, 0: failure, 2: mode error).
     * @param timeout Operation timeout in milliseconds (default 100 ms).
     * @return Operation status code (e.g., CHAIN_OK, CHAIN_PARAMETER_ERROR).
     */
    chain_status_t setBuzzerNotePlay(uint8_t id, note_index_t note, uint16_t duration, uint8_t *operationStatus,
                                     unsigned long timeout = 100);

private:
};

#endif  // _CHAIN_BUZZER_HPP_

/*
 *SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#include "M5Chain.h"

#define TXD_PIN GPIO_NUM_21  // Tx
#define RXD_PIN GPIO_NUM_22  // Rx

Chain M5Chain;

device_list_t *devices_list = NULL;
uint16_t device_nums        = 0;
uint8_t operation_status    = 0;
chain_status_t chain_status = CHAIN_OK;

uint8_t rgb_test[5][3] = {
    {0xFF, 0x00, 0x00}, {0x00, 0xFF, 0x00}, {0x00, 0x00, 0xFF}, {0xFF, 0xFF, 0xFF}, {0x00, 0x00, 0x00},
};

// Set Tempo (Beats Per Minute)
#define BPM 200

// Define a structure to represent a note in the melody
struct Melody {
    note_index_t note;  // Note pitch, corresponding to your enum
    float beats;        // Duration in beats (e.g., 1.0 = Quarter Note, 0.5 = Eighth Note)
};

// Super Mario Bros. BGM Melody
// Note duration: 0.5 = Eighth Note, 1.0 = Quarter Note
const Melody marioBGM[] = {

    // 1. Intro
    {NOTE_E5, 0.5},
    {NOTE_E5, 0.5},
    {NOTE_REST, 0.5},
    {NOTE_E5, 0.5},
    {NOTE_REST, 0.5},
    {NOTE_C5, 0.5},
    {NOTE_E5, 1.0},
    {NOTE_G5, 1.0},
    {NOTE_REST, 1.0},
    {NOTE_G4, 1.0},
    {NOTE_REST, 1.0},

    // 2. Theme A (Main Theme) - First Pass
    {NOTE_C5, 1.5},
    {NOTE_G4, 0.5},
    {NOTE_REST, 1.0},
    {NOTE_E4, 1.0},
    {NOTE_REST, 0.5},
    {NOTE_A4, 1.0},
    {NOTE_B4, 1.0},
    {NOTE_AS4, 0.5},
    {NOTE_A4, 1.0},

    {NOTE_G4, 0.66},
    {NOTE_E5, 0.66},
    {NOTE_G5, 0.66},  // Triplet feel
    {NOTE_A5, 1.0},
    {NOTE_F5, 0.5},
    {NOTE_G5, 0.5},
    {NOTE_REST, 0.5},
    {NOTE_E5, 1.0},
    {NOTE_C5, 0.5},
    {NOTE_D5, 0.5},
    {NOTE_B4, 0.5},
    {NOTE_REST, 1.0},

    // Theme A (Main Theme) - Second Pass (Repeat)
    {NOTE_C5, 1.5},
    {NOTE_G4, 0.5},
    {NOTE_REST, 1.0},
    {NOTE_E4, 1.0},
    {NOTE_REST, 0.5},
    {NOTE_A4, 1.0},
    {NOTE_B4, 1.0},
    {NOTE_AS4, 0.5},
    {NOTE_A4, 1.0},

    {NOTE_G4, 0.66},
    {NOTE_E5, 0.66},
    {NOTE_G5, 0.66},
    {NOTE_A5, 1.0},
    {NOTE_F5, 0.5},
    {NOTE_G5, 0.5},
    {NOTE_REST, 0.5},
    {NOTE_E5, 1.0},
    {NOTE_C5, 0.5},
    {NOTE_D5, 0.5},
    {NOTE_B4, 0.5},
    {NOTE_REST, 1.0},

    // 3. Theme B (Secondary Theme) - First Pass
    {NOTE_REST, 0.5},
    {NOTE_G5, 0.5},
    {NOTE_FS5, 0.5},
    {NOTE_F5, 0.5},
    {NOTE_DS5, 0.5},
    {NOTE_E5, 0.5},
    {NOTE_REST, 0.5},
    {NOTE_GS4, 0.5},
    {NOTE_A4, 0.5},
    {NOTE_C5, 0.5},
    {NOTE_REST, 0.5},
    {NOTE_A4, 0.5},
    {NOTE_C5, 0.5},
    {NOTE_D5, 0.5},

    {NOTE_REST, 0.5},
    {NOTE_G5, 0.5},
    {NOTE_FS5, 0.5},
    {NOTE_F5, 0.5},
    {NOTE_DS5, 0.5},
    {NOTE_E5, 0.5},
    {NOTE_REST, 0.5},
    {NOTE_C6, 1.0},
    {NOTE_REST, 0.5},
    {NOTE_C6, 1.0},
    {NOTE_C6, 1.0},  // Final three high C notes
    {NOTE_REST, 1.0},

    // Theme B (Secondary Theme) - Second Pass
    {NOTE_REST, 0.5},
    {NOTE_G5, 0.5},
    {NOTE_FS5, 0.5},
    {NOTE_F5, 0.5},
    {NOTE_DS5, 0.5},
    {NOTE_E5, 0.5},
    {NOTE_REST, 0.5},
    {NOTE_GS4, 0.5},
    {NOTE_A4, 0.5},
    {NOTE_C5, 0.5},
    {NOTE_REST, 0.5},
    {NOTE_A4, 0.5},
    {NOTE_C5, 0.5},
    {NOTE_D5, 0.5},

    {NOTE_REST, 0.5},
    {NOTE_DS5, 0.5},
    {NOTE_REST, 0.5},
    {NOTE_D5, 0.5},
    {NOTE_REST, 0.5},
    {NOTE_C5, 1.0},
    {NOTE_REST, 2.0}  // Long rest before the loop ends
};

// Calculate the number of notes in the melody
const int melodyLen = sizeof(marioBGM) / sizeof(marioBGM[0]);

void printDeviceList(device_list_t *devices)
{
    if (devices == NULL) {
        Serial.println("devices is NULL");
        return;
    }

    Serial.print("devices count: ");
    Serial.println(devices->count);

    for (uint8_t i = 0; i < devices->count; i++) {
        Serial.print("devices ID: ");
        Serial.println(devices->devices[i].id);
        Serial.print("devices type: ");
        Serial.println(devices->devices[i].device_type);
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("M5Chain Buzzer Test");
    M5Chain.begin(&Serial2, 115200, RXD_PIN, TXD_PIN);

    if (M5Chain.isDeviceConnected()) {
        Serial.println("devices is connected");
        chain_status = M5Chain.getDeviceNum(&device_nums);
        if (chain_status == CHAIN_OK) {
            devices_list          = (device_list_t *)malloc(sizeof(device_list_t));
            devices_list->count   = device_nums;
            devices_list->devices = (device_info_t *)malloc(sizeof(device_info_t) * device_nums);
            if (M5Chain.getDeviceList(devices_list)) {
                Serial.println("get devices list success");
                printDeviceList(devices_list);
            } else {
                Serial.println("get devices list failed");
            }
        } else {
            Serial.printf("error status:%d \r\n", chain_status);
            Serial.printf("devices num get failed.\r\n");
        }
    } else {
        Serial.println("devices is not connected.");
    }

    if (devices_list) {
        for (uint8_t i = 0; i < devices_list->count; i++) {
            if (devices_list->devices[i].device_type == CHAIN_BUZZER_TYPE_CODE) {
                chain_status = M5Chain.setRGBLight(devices_list->devices[i].id, 100, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("Buzzer ID[%d] set rgb light success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("Buzzer ID[%d] set rgb light failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }
                for (uint8_t j = 0; j < 5; j++) {
                    uint8_t rgb[3] = {0};
                    chain_status =
                        M5Chain.setRGBValue(devices_list->devices[i].id, 0, 1, rgb_test[j], 3, &operation_status);
                    if (chain_status == CHAIN_OK && operation_status) {
                        Serial.printf("Buzzer ID[%d] set rgb %d %d %d success\r\n", devices_list->devices[i].id,
                                      rgb_test[j][0], rgb_test[j][1], rgb_test[j][2]);
                    } else {
                        Serial.printf(
                            "Buzzer ID[%d] set rgb %d %d %d failed, chain_status:%d  operation_status:%d \r\n",
                            devices_list->devices[i].id, rgb_test[j][0], rgb_test[j][1], rgb_test[j][2], chain_status,
                            operation_status);
                    }
                    chain_status = M5Chain.getRGBValue(devices_list->devices[i].id, 0, 1, rgb, 3, &operation_status);
                    if (chain_status == CHAIN_OK && operation_status) {
                        Serial.printf("Buzzer ID[%d] get rgb %d %d %d success \r\n", devices_list->devices[i].id,
                                      rgb[0], rgb[1], rgb[2]);
                    } else {
                        Serial.printf(
                            "Buzzer ID[%d] get rgb %d %d %d failed, chain_status:%d  operation_status:%d \r\n",
                            devices_list->devices[i].id, rgb[0], rgb[1], rgb[2], chain_status, operation_status);
                    }
                    delay(500);
                }

                chain_status =
                    M5Chain.setBuzzerMode(devices_list->devices[i].id, BUZZER_MODE_NOTE_PLAY, &operation_status);
                if (chain_status == CHAIN_OK && operation_status) {
                    Serial.printf("Buzzer ID[%d] set mode success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("Buzzer ID[%d] set mode failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }
            }
        }
    } else {
        Serial.println("devices list is NULL");
    }
}

void loop()
{
    operation_status = 0;
    if (devices_list) {
        for (uint8_t i = 0; i < devices_list->count; i++) {
            if (devices_list->devices[i].device_type == CHAIN_BUZZER_TYPE_CODE) {
                // Calculate the duration of one beat (quarter note) in milliseconds
                long quarterNoteMs = 60000 / BPM;
                for (int j = 0; j < melodyLen; j++) {
                    // 1. Get the current note and beat count
                    note_index_t currentNote = marioBGM[j].note;
                    float currentBeats       = marioBGM[j].beats;

                    // 2. Convert beats to milliseconds
                    uint16_t durationMs = currentBeats * quarterNoteMs;

                    // 3. Handle note playback
                    if (currentNote == NOTE_REST) {
                        delay(durationMs);
                    } else {
                        chain_status = M5Chain.setBuzzerNotePlay(devices_list->devices[i].id, currentNote, durationMs,
                                                                 &operation_status);
                        if (chain_status == CHAIN_OK && operation_status == 1) {
                            Serial.printf("Buzzer ID[%d] set note play success note: %s , duration: %d ms\r\n",
                                          devices_list->devices[i].id, note_names[currentNote], durationMs);
                        } else {
                            Serial.printf(
                                "Buzzer ID[%d] set buzzer note play failed, chain_status:%d  operation_status:%d\r\n",
                                devices_list->devices[i].id, chain_status, operation_status);
                        }

                        // 4. Wait for the note playback to finish
                        delay(durationMs * 1.30);
                    }
                }
            }
        }
    }
}

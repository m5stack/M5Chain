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

// Effect 1: Matrix Rain
class MatrixRainEffect {
private:
    struct Column {
        int8_t head_y;
        uint8_t length;
        uint8_t speed;
        bool active;
    };

    Column columns[8];
    uint32_t last_update;

public:
    uint16_t pattern[64];

    MatrixRainEffect()
    {
        last_update = 0;
        memset(pattern, 0, sizeof(pattern));
        for (int i = 0; i < 8; i++) {
            columns[i].head_y = random(-8, 0);
            columns[i].length = random(3, 6);
            columns[i].speed  = random(1, 4);
            columns[i].active = random(0, 2);
        }
    }

    void update()
    {
        if (millis() - last_update < 100) return;
        last_update = millis();

        memset(pattern, 0, sizeof(pattern));

        for (int col = 0; col < 8; col++) {
            if (!columns[col].active) {
                if (random(0, 10) < 2) {
                    columns[col].active = true;
                    columns[col].head_y = 0;
                    columns[col].length = random(3, 6);
                }
                continue;
            }

            for (int i = 0; i < columns[col].length; i++) {
                int y = columns[col].head_y - i;
                if (y >= 0 && y < 8) {
                    uint16_t color;
                    if (i == 0) {
                        color = 0x2FE5;  // Brightest green
                    } else if (i == 1) {
                        color = 0x1FE3;  // Very bright green
                    } else if (i == 2) {
                        color = 0x07E0;  // Standard green
                    } else if (i == 3) {
                        color = 0x05C0;  // Medium green
                    } else {
                        color = 0x0320;  // Dark green
                    }
                    pattern[y * 8 + col] = color;
                }
            }

            columns[col].head_y++;

            if (columns[col].head_y - columns[col].length >= 8) {
                columns[col].active = false;
                columns[col].head_y = random(-5, 0);
            }
        }
    }
};

// Effect 2: Random Bar Graph Effect
class RandomBarEffect {
private:
    int target_y[8];   // Target height
    int current_y[8];  // Current height
    int max_y[8];      // Peak position
    uint32_t last_update;
    uint32_t last_random_update;
    uint32_t frame_count;

    // 8-color gradient: Blue->Cyan->Green->Yellow->Red
    const uint16_t colors[8] = {
        0x001F,  // Blue
        0x03FF,  // Cyan-blue
        0x07FF,  // Cyan
        0x07E0,  // Green
        0xBFE0,  // Yellow-green
        0xFFE0,  // Yellow
        0xFD20,  // Orange
        0xF800   // Red
    };

public:
    uint16_t pattern[64];

    RandomBarEffect()
    {
        last_update        = 0;
        last_random_update = 0;
        frame_count        = 0;
        memset(pattern, 0, sizeof(pattern));

        for (int x = 0; x < 8; x++) {
            target_y[x]  = random(0, 8);
            current_y[x] = 7;
            max_y[x]     = 7;
        }
    }

    void update()
    {
        if (millis() - last_update < 50) return;
        last_update = millis();

        frame_count++;

        if (millis() - last_random_update > 150) {
            last_random_update = millis();
            for (int x = 0; x < 8; x++) {
                if (random(0, 100) < 70) {
                    target_y[x] = random(0, 8);
                }
            }
        }

        memset(pattern, 0, sizeof(pattern));

        for (int x = 0; x < 8; x++) {
            if (current_y[x] < target_y[x]) {
                current_y[x]++;
            } else if (current_y[x] > target_y[x]) {
                current_y[x]--;
            }

            int y = current_y[x];
            if (y < 0) y = 0;
            if (y > 7) y = 7;

            if (y <= max_y[x]) {
                max_y[x] = y;
            } else {
                if (frame_count % 6 == 0) {
                    max_y[x]++;
                    if (max_y[x] > 7) max_y[x] = 7;
                }
            }

            for (int row = y; row <= 7; row++) {
                pattern[row * 8 + x] = colors[row > 7 ? 7 : row];
            }

            if (max_y[x] >= 0 && max_y[x] <= 7) {
                pattern[max_y[x] * 8 + x] = 0xFFFF;
            }
        }
    }
};

// Effect 3: Color Waterfall Effect
class ColorWaterfallEffect {
private:
    uint32_t last_update;
    float hue_offset;
    uint8_t scroll_counter;

    uint16_t HSVtoRGB565(float h, float s, float v)
    {
        float r, g, b;

        int i   = (int)(h * 6.0);
        float f = h * 6.0 - i;
        float p = v * (1.0 - s);
        float q = v * (1.0 - f * s);
        float t = v * (1.0 - (1.0 - f) * s);

        switch (i % 6) {
            case 0:
                r = v;
                g = t;
                b = p;
                break;
            case 1:
                r = q;
                g = v;
                b = p;
                break;
            case 2:
                r = p;
                g = v;
                b = t;
                break;
            case 3:
                r = p;
                g = q;
                b = v;
                break;
            case 4:
                r = t;
                g = p;
                b = v;
                break;
            case 5:
                r = v;
                g = p;
                b = q;
                break;
        }

        uint8_t r8 = r * 255;
        uint8_t g8 = g * 255;
        uint8_t b8 = b * 255;

        return ((r8 & 0xF8) << 8) | ((g8 & 0xFC) << 3) | (b8 >> 3);
    }

public:
    uint16_t pattern[64];

    ColorWaterfallEffect()
    {
        last_update    = 0;
        hue_offset     = 0;
        scroll_counter = 0;
        memset(pattern, 0, sizeof(pattern));
    }

    void update()
    {
        if (millis() - last_update < 80) return;
        last_update = millis();

        scroll_counter++;

        if (scroll_counter % 2 == 0) {
            // Scroll down
            for (int y = 7; y > 0; y--) {
                for (int x = 0; x < 8; x++) {
                    pattern[y * 8 + x] = pattern[(y - 1) * 8 + x];
                }
            }
        }

        hue_offset += 0.005;
        if (hue_offset >= 1.0) hue_offset = 0;

        for (int x = 0; x < 8; x++) {
            float hue        = fmod(hue_offset + (float)x / 16.0, 1.0);
            float saturation = 1.0;
            float value      = 0.85 + sin(hue_offset * 3.14 + x * 0.3) * 0.15;

            pattern[x] = HSVtoRGB565(hue, saturation, value);
        }
    }
};

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
    Serial.println("M5Chain RGB Test");
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
            if (devices_list->devices[i].device_type == CHAIN_RGB_TYPE_CODE) {
                chain_status = M5Chain.setRGBMode(devices_list->devices[i].id, RGB_PIXEL_MODE, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("RGB ID[%d] set pixel mode success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("RGB ID[%d] set pixel mode failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }
                chain_status = M5Chain.setRGBRotation(devices_list->devices[i].id, RGB_ROTATION_0, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("RGB ID[%d] set rotation success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("RGB ID[%d] set rotation failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                chain_status = M5Chain.setRGBBrightness(devices_list->devices[i].id, 50, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("RGB ID[%d] set brightness success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("RGB ID[%d] set brightness failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }
            }
        }
    } else {
        Serial.println("devices list is NULL");
    }
}

MatrixRainEffect matrixraineffect;
RandomBarEffect randombareffect;
ColorWaterfallEffect coloewatereffect;

void loop()
{
    if (devices_list) {
        for (uint8_t i = 0; i < devices_list->count; i++) {
            if (devices_list->devices[i].device_type == CHAIN_RGB_TYPE_CODE) {
                rgb_mode_t rgb_mode;
                chain_status = M5Chain.getRGBMode(devices_list->devices[i].id, &rgb_mode);
                if (chain_status == CHAIN_OK) {
                    Serial.printf("RGB ID[%d] rgb mode:%d\r\n", devices_list->devices[i].id, rgb_mode);
                } else {
                    Serial.printf("RGB ID[%d] get mode failed, chain_status:%d \r\n", devices_list->devices[i].id,
                                  chain_status);
                }

                chain_status = M5Chain.setRGBClear(devices_list->devices[i].id, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("RGB ID[%d] set clear success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("RGB ID[%d] set clear failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                for (uint8_t j = 0; j < 8; j++) {
                    for (uint8_t k = 0; k < 8; k++) {
                        chain_status =
                            M5Chain.setRGBPixel(devices_list->devices[i].id, k, j, 0xFFFF, &operation_status);
                        if (chain_status == CHAIN_OK && operation_status == 1) {
                            Serial.printf("RGB ID[%d] set pixel success, x:%d, y:%d\r\n", devices_list->devices[i].id,
                                          k, j);
                        } else {
                            Serial.printf("RGB ID[%d] set pixel failed, chain_status:%d  operation_status:%d \r\n",
                                          devices_list->devices[i].id, chain_status, operation_status);
                        }
                        delay(30);
                    }
                }
                delay(100);
                chain_status = M5Chain.setRGBClear(devices_list->devices[i].id, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("RGB ID[%d] set clear success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("RGB ID[%d] set clear failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                RGBPixelInfo pattern_rainbow[] = {
                    // Row 0 - Red
                    {1, 0, 0xF800},
                    {2, 0, 0xF800},
                    {3, 0, 0xF800},
                    {4, 0, 0xF800},
                    {5, 0, 0xF800},
                    {6, 0, 0xF800},
                    // Row 1 - Orange
                    {1, 1, 0xFD20},
                    {2, 1, 0xFD20},
                    {3, 1, 0xFD20},
                    {4, 1, 0xFD20},
                    {5, 1, 0xFD20},
                    {6, 1, 0xFD20},
                    // Row 2 - Yellow
                    {1, 2, 0xFFE0},
                    {2, 2, 0xFFE0},
                    {3, 2, 0xFFE0},
                    {4, 2, 0xFFE0},
                    {5, 2, 0xFFE0},
                    {6, 2, 0xFFE0},
                    // Row 3 - Green
                    {1, 3, 0x07E0},
                    {2, 3, 0x07E0},
                    {3, 3, 0x07E0},
                    {4, 3, 0x07E0},
                    {5, 3, 0x07E0},
                    {6, 3, 0x07E0},
                    // Row 4 - Cyan
                    {1, 4, 0x07FF},
                    {2, 4, 0x07FF},
                    {3, 4, 0x07FF},
                    {4, 4, 0x07FF},
                    {5, 4, 0x07FF},
                    {6, 4, 0x07FF},
                    // Row 5 - Blue
                    {1, 5, 0x001F},
                    {2, 5, 0x001F},
                    {3, 5, 0x001F},
                    {4, 5, 0x001F},
                    {5, 5, 0x001F},
                    {6, 5, 0x001F},
                    // Row 6 - Purple
                    {1, 6, 0xF81F},
                    {2, 6, 0xF81F},
                    {3, 6, 0xF81F},
                    {4, 6, 0xF81F},
                    {5, 6, 0xF81F},
                    {6, 6, 0xF81F},
                    // Row 7 - Pink
                    {1, 7, 0xFE19},
                    {2, 7, 0xFE19},
                    {3, 7, 0xFE19},
                    {4, 7, 0xFE19},
                    {5, 7, 0xFE19},
                    {6, 7, 0xFE19},
                };

                chain_status =
                    M5Chain.setRGBPixel(devices_list->devices[i].id, pattern_rainbow,
                                        sizeof(pattern_rainbow) / sizeof(pattern_rainbow[0]), &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("RGB ID[%d] set pixels success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("RGB ID[%d] set pixels failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }
                delay(2000);
                chain_status = M5Chain.setRGBClear(devices_list->devices[i].id, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("RGB ID[%d] set clear success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("RGB ID[%d] set clear failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                for (char c = 'A'; c <= 'Z'; ++c) {
                    // Generate color based on letter position (rainbow gradient)
                    int pos     = c - 'A';
                    uint8_t hue = (pos * 255) / 26;  // 0-255
                    // Simple HSV to RGB conversion
                    uint32_t color;
                    if (hue < 85) {
                        color = (hue * 3) << 16 | ((85 - hue) * 3) << 8;
                    } else if (hue < 170) {
                        color = ((170 - hue) * 3) << 8 | ((hue - 85) * 3);
                    } else {
                        color = ((hue - 170) * 3) | ((255 - hue) * 3) << 16;
                    }

                    chain_status =
                        M5Chain.setRGBPrintChar(devices_list->devices[i].id, c, 1, 1, color, &operation_status);
                    if (chain_status == CHAIN_OK && operation_status == 1) {
                        Serial.printf("RGB ID[%d] set char success, ascii:%c, color:0x%06X\r\n",
                                      devices_list->devices[i].id, c, color);
                    } else {
                        Serial.printf("RGB ID[%d] set char failed, chain_status:%d  operation_status:%d \r\n",
                                      devices_list->devices[i].id, chain_status, operation_status);
                    }
                    delay(200);
                }
                delay(1000);

                chain_status = M5Chain.setRGBClear(devices_list->devices[i].id, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("RGB ID[%d] set clear success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("RGB ID[%d] set clear failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                uint16_t pattern_arrow[64] = {// Y0 - Arrow tip
                                              0x0000, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000,
                                              // Y1
                                              0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000,
                                              // Y2
                                              0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x0000,
                                              // Y3
                                              0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0,
                                              // Y4 - Arrow body
                                              0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000,
                                              // Y5
                                              0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000,
                                              // Y6
                                              0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000,
                                              // Y7
                                              0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000};

                chain_status =
                    M5Chain.setRGBBufferRefresh(devices_list->devices[i].id, pattern_arrow, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("RGB ID[%d] set buffer refresh success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("RGB ID[%d] set buffer refresh failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }

                for (uint8_t j = 0; j <= (RGB_ROTATION_270 + 1); j++) {
                    chain_status = M5Chain.setRGBRotation(
                        devices_list->devices[i].id, (rgb_rotation_t)(j % (RGB_ROTATION_270 + 1)), &operation_status);
                    if (chain_status == CHAIN_OK && operation_status == 1) {
                        Serial.printf("RGB ID[%d] set rotation success\r\n", devices_list->devices[i].id);
                    } else {
                        Serial.printf("RGB ID[%d] set rotation failed, chain_status:%d  operation_status:%d \r\n",
                                      devices_list->devices[i].id, chain_status, operation_status);
                    }
                    delay(1000);
                }

                for (uint8_t j = 1; j <= 100; j++) {
                    chain_status = M5Chain.setRGBBrightness(devices_list->devices[i].id, j, &operation_status);
                    if (chain_status == CHAIN_OK && operation_status == 1) {
                        Serial.printf("RGB ID[%d] set brightness success\r\n", devices_list->devices[i].id);
                    } else {
                        Serial.printf("RGB ID[%d] set brightness failed, chain_status:%d  operation_status:%d \r\n",
                                      devices_list->devices[i].id, chain_status, operation_status);
                    }
                    delay(20);
                }

                for (uint8_t j = 100; j >= 50; j--) {
                    chain_status = M5Chain.setRGBBrightness(devices_list->devices[i].id, j, &operation_status);
                    if (chain_status == CHAIN_OK && operation_status == 1) {
                        Serial.printf("RGB ID[%d] set brightness success\r\n", devices_list->devices[i].id);
                    } else {
                        Serial.printf("RGB ID[%d] set brightness failed, chain_status:%d  operation_status:%d \r\n",
                                      devices_list->devices[i].id, chain_status, operation_status);
                    }
                    delay(20);
                }
                delay(1000);

                uint32_t last_millis = millis();
                while (millis() - last_millis < 10000) {
                    matrixraineffect.update();
                    chain_status = M5Chain.setRGBBufferRefresh(devices_list->devices[i].id, matrixraineffect.pattern,
                                                               &operation_status);
                    if (chain_status == CHAIN_OK && operation_status == 1) {
                    } else {
                        Serial.printf("RGB ID[%d] set buffer refresh failed, chain_status:%d  operation_status:%d \r\n",
                                      devices_list->devices[i].id, chain_status, operation_status);
                    }
                }

                last_millis = millis();
                while (millis() - last_millis < 10000) {
                    randombareffect.update();
                    chain_status = M5Chain.setRGBBufferRefresh(devices_list->devices[i].id, randombareffect.pattern,
                                                               &operation_status);
                    if (chain_status == CHAIN_OK && operation_status == 1) {
                    } else {
                        Serial.printf("RGB ID[%d] set buffer refresh failed, chain_status:%d  operation_status:%d \r\n",
                                      devices_list->devices[i].id, chain_status, operation_status);
                    }
                }

                last_millis = millis();
                while (millis() - last_millis < 10000) {
                    coloewatereffect.update();
                    chain_status = M5Chain.setRGBBufferRefresh(devices_list->devices[i].id, coloewatereffect.pattern,
                                                               &operation_status);
                    if (chain_status == CHAIN_OK && operation_status == 1) {
                    } else {
                        Serial.printf("RGB ID[%d] set buffer refresh failed, chain_status:%d  operation_status:%d \r\n",
                                      devices_list->devices[i].id, chain_status, operation_status);
                    }
                }

                chain_status = M5Chain.setRGBClear(devices_list->devices[i].id, &operation_status);
                if (chain_status == CHAIN_OK && operation_status == 1) {
                    Serial.printf("RGB ID[%d] set clear success\r\n", devices_list->devices[i].id);
                } else {
                    Serial.printf("RGB ID[%d] set clear failed, chain_status:%d  operation_status:%d \r\n",
                                  devices_list->devices[i].id, chain_status, operation_status);
                }
            }
        }
    }
    delay(100);
}

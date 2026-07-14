/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "M5Chain.h"

// Serial Pin Definitions / 串口引脚定义
#define TXD_PIN (GPIO_NUM_32) /**< Transmit Pin / 发送引脚 */
#define RXD_PIN (GPIO_NUM_33) /**< Receive Pin / 接收引脚 */

// Configuration Constants / 配置常量
#define RGB_CHANNEL_COUNT   (8)    /**< Number of RGB channels / RGB通道数量 */
#define RGB_LED_COUNT       (16)   /**< Number of RGB LEDs (2 per channel) / RGB LED数量(每通道2个) */
#define RGB_COLOR_COUNT     (5)    /**< Number of test colors / 测试颜色数量 */
#define RGB_CONFIG_BASE     (0x20) /**< RGB configuration base value / RGB配置基准值 */
#define RGB_CONFIG_LOOP_MAX (16)   /**< Maximum RGB config loop count / RGB配置循环最大次数 */

// Global Objects and Variables / 全局对象和变量
Chain M5Chain;                          /**< M5Chain instance / M5Chain实例 */
device_list_t *devices_list = NULL;     /**< Device list pointer / 设备列表指针 */
uint16_t device_nums        = 0;        /**< Device count / 设备数量 */
uint8_t operation_status    = 0;        /**< Operation status / 操作状态 */
chain_status_t chain_status = CHAIN_OK; /**< Chain status / 链路状态 */

// RGB Test Configuration / RGB测试配置
uint8_t test_index                   = 0; /**< Current test color index / 当前测试颜色索引 */
uint32_t test_color[RGB_COLOR_COUNT] = {
    0xFF0000, /**< Red / 红色 */
    0x00FF00, /**< Green / 绿色 */
    0x0000FF, /**< Blue / 蓝色 */
    0xFFFFFF, /**< White / 白色 */
    0x000000  /**< Off / 关闭 */
};

// Function Declarations / 函数声明
void printDeviceList(device_list_t *devices);
void updateDeviceList();
bool initializeRgbMode();
bool testRgbFunctionality();

void setup()
{
    Serial.begin(115200);
    Serial.println("M5Chain Unit 8Servos2 Chain RGB Test");

    // Initialize M5Chain communication / 初始化M5Chain通信
    M5Chain.begin(&Serial2, 115200, RXD_PIN, TXD_PIN);

    // Update device list / 更新设备列表
    updateDeviceList();

    // Initialize RGB mode / 初始化RGB模式
    if (!initializeRgbMode()) {
        Serial.println("RGB mode initialization failed / RGB模式初始化失败");
    }
}

void loop()
{
    // Validate device list validity / 验证设备列表有效性
    if (!devices_list) {
        Serial.println("Device list is NULL / 设备列表为空");
        return;
    }

    // Test RGB functionality / 测试RGB功能
    if (!testRgbFunctionality()) {
        Serial.println("RGB functionality test failed / RGB功能测试失败");
    }
}

/**
 * @brief Initialize RGB mode for all 8Servos2 devices / 初始化所有8Servos2设备的RGB模式
 *
 * Set all channels to RGB mode individually / 逐个设置所有通道为RGB模式
 *
 * @return true if successful / 成功返回true
 * @return false if failed / 失败返回false
 */
bool initializeRgbMode()
{
    if (!devices_list) {
        Serial.println("Device list is NULL, cannot initialize / 设备列表为空，无法初始化");
        return false;
    }

    bool all_success = true;

    // Iterate through all devices / 遍历所有设备
    for (uint8_t i = 0; i < devices_list->count; i++) {
        // Check device type / 检查设备类型
        if (devices_list->devices[i].device_type != UNIT_8SERVOS2_CHAIN_TYPE_CODE) {
            continue;
        }

        uint8_t device_id = devices_list->devices[i].id;

        // Set RGB mode for each channel / 为每个通道设置RGB模式
        for (uint8_t j = 0; j < RGB_CHANNEL_COUNT; j++) {
            chain_status = M5Chain.setServosMode(device_id, j, USER_GPIO_RGB_MODE, &operation_status);

            // Validate write operation / 验证写入操作
            if (chain_status != CHAIN_OK) {
                Serial.printf(
                    "Device ID[%d] channel %d set RGB mode failed (communication error) / "
                    "设备ID[%d]通道%d设置RGB模式失败(通信错误)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                continue;
            }

            if (operation_status != 1) {
                Serial.printf(
                    "Device ID[%d] channel %d set RGB mode failed (operation status error) / "
                    "设备ID[%d]通道%d设置RGB模式失败(操作状态异常)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                continue;
            }

            Serial.printf("Device ID[%d] channel %d set RGB mode success / 设备ID[%d]通道%d设置RGB模式成功\r\n",
                          device_id, j, device_id, j);
        }
    }

    return all_success;
}

/**
 * @brief Test RGB functionality for all devices / 测试所有设备的RGB功能
 *
 * Process: Verify mode -> Set/Get RGB buffer -> Set/Get RGB config /
 * 流程：验证模式 -> 设置/获取RGB缓冲 -> 设置/获取RGB配置
 *
 * @return true if successful / 成功返回true
 * @return false if failed / 失败返回false
 */
bool testRgbFunctionality()
{
    bool all_success = true;

    // Iterate through all devices / 遍历所有设备
    for (uint8_t i = 0; i < devices_list->count; i++) {
        // Check device type / 检查设备类型
        if (devices_list->devices[i].device_type != UNIT_8SERVOS2_CHAIN_TYPE_CODE) {
            continue;
        }

        uint8_t device_id = devices_list->devices[i].id;

        // Step 1: Verify RGB mode for all channels / 步骤1：验证所有通道RGB模式
        for (uint8_t j = 0; j < RGB_CHANNEL_COUNT; j++) {
            user_gpio_mode_t mode;

            chain_status = M5Chain.getServosMode(device_id, j, &mode, &operation_status);

            if (chain_status != CHAIN_OK) {
                Serial.printf(
                    "Device ID[%d] channel %d get RGB mode failed (communication error) / "
                    "设备ID[%d]通道%d获取RGB模式失败(通信错误)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                break;
            }

            if (operation_status != 1) {
                Serial.printf(
                    "Device ID[%d] channel %d get RGB mode failed (operation status error) / "
                    "设备ID[%d]通道%d获取RGB模式失败(操作状态异常)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                break;
            }

            if (mode != USER_GPIO_RGB_MODE) {
                Serial.printf("Device ID[%d] channel %d RGB mode mismatch / 设备ID[%d]通道%d RGB模式不匹配\r\n",
                              device_id, j, device_id, j);
                all_success = false;
                break;
            }

            // Step 2: Set and verify RGB buffer for all LEDs / 步骤2：设置并验证所有LED的RGB缓冲
            for (uint8_t k = 0; k < RGB_LED_COUNT; k++) {
                // Write RGB buffer / 写入RGB缓冲
                chain_status = M5Chain.setServosRGBBuffer(device_id, k, test_color[test_index], &operation_status);

                if (chain_status != CHAIN_OK) {
                    Serial.printf(
                        "Device ID[%d] LED %d set RGB buffer failed (communication error) / 设备ID[%d] LED "
                        "%d设置RGB缓冲失败(通信错误)\r\n",
                        device_id, k, device_id, k);
                    all_success = false;
                    break;
                }

                if (operation_status != 1) {
                    Serial.printf(
                        "Device ID[%d] LED %d set RGB buffer failed (operation status error) / 设备ID[%d] LED "
                        "%d设置RGB缓冲失败(操作状态异常)\r\n",
                        device_id, k, device_id, k);
                    all_success = false;
                    break;
                }

                // Read and verify RGB buffer / 读取并验证RGB缓冲
                uint32_t get_color = 0;
                chain_status       = M5Chain.getServosRGBBuffer(device_id, k, &get_color, &operation_status);

                if (chain_status != CHAIN_OK) {
                    Serial.printf(
                        "Device ID[%d] LED %d get RGB buffer failed (communication error) / 设备ID[%d] LED "
                        "%d获取RGB缓冲失败(通信错误)\r\n",
                        device_id, k, device_id, k);
                    all_success = false;
                    break;
                }

                if (operation_status != 1) {
                    Serial.printf(
                        "Device ID[%d] LED %d get RGB buffer failed (operation status error) / 设备ID[%d] LED "
                        "%d获取RGB缓冲失败(操作状态异常)\r\n",
                        device_id, k, device_id, k);
                    all_success = false;
                    break;
                }

                if (get_color != test_color[test_index]) {
                    Serial.printf(
                        "Device ID[%d] LED %d color mismatch: expected 0x%06X, got 0x%06X / 设备ID[%d] LED "
                        "%d颜色不匹配：期望0x%06X，实际0x%06X\r\n",
                        device_id, k, test_color[test_index], get_color, device_id, k, test_color[test_index],
                        get_color);
                    all_success = false;
                    break;
                }
            }

            // Cycle through test colors / 循环切换测试颜色
            test_index = (test_index + 1) % RGB_COLOR_COUNT;

            // Step 3: Test RGB configuration with different brightness levels / 步骤3：测试不同亮度级别的RGB配置
            for (uint8_t k = 1; k <= RGB_CONFIG_LOOP_MAX; k++) {
                uint8_t rgb_config[RGB_CHANNEL_COUNT];

                // Prepare RGB configuration / 准备RGB配置
                for (uint8_t w = 0; w < RGB_CHANNEL_COUNT; w++) {
                    rgb_config[w] = RGB_CONFIG_BASE + k;
                }

                // Set and verify RGB config for all channels / 设置并验证所有通道的RGB配置
                for (uint8_t h = 0; h < RGB_CHANNEL_COUNT; h++) {
                    // Write RGB configuration / 写入RGB配置
                    chain_status = M5Chain.setServosRGBConfig(device_id, h, rgb_config[h], &operation_status);

                    if (chain_status != CHAIN_OK) {
                        Serial.printf(
                            "Device ID[%d] channel %d set RGB config failed (communication error) / "
                            "设备ID[%d]通道%d设置RGB配置失败(通信错误)\r\n",
                            device_id, h, device_id, h);
                        all_success = false;
                        break;
                    }

                    if (operation_status != 1) {
                        Serial.printf(
                            "Device ID[%d] channel %d set RGB config failed (operation status error) / "
                            "设备ID[%d]通道%d设置RGB配置失败(操作状态异常)\r\n",
                            device_id, h, device_id, h);
                        all_success = false;
                        break;
                    }

                    // Read and verify RGB configuration / 读取并验证RGB配置
                    uint8_t rgb_config_get = 0;
                    chain_status = M5Chain.getServosRGBConfig(device_id, h, &rgb_config_get, &operation_status);

                    if (chain_status != CHAIN_OK) {
                        Serial.printf(
                            "Device ID[%d] channel %d get RGB config failed (communication error) / "
                            "设备ID[%d]通道%d获取RGB配置失败(通信错误)\r\n",
                            device_id, h, device_id, h);
                        all_success = false;
                        break;
                    }

                    if (operation_status != 1) {
                        Serial.printf(
                            "Device ID[%d] channel %d get RGB config failed (operation status error) / "
                            "设备ID[%d]通道%d获取RGB配置失败(操作状态异常)\r\n",
                            device_id, h, device_id, h);
                        all_success = false;
                        break;
                    }

                    // Verify RGB configuration / 验证RGB配置
                    // Note: Config value is stored without base offset / 注意：配置值存储时不含基准偏移
                    if (rgb_config_get != (rgb_config[h] - RGB_CONFIG_BASE)) {
                        Serial.printf(
                            "Device ID[%d] channel %d RGB config mismatch: expected %d, got %d / 设备ID[%d]通道%d "
                            "RGB配置不匹配：期望%d，实际%d\r\n",
                            device_id, h, rgb_config[h] - RGB_CONFIG_BASE, rgb_config_get, device_id, h,
                            rgb_config[h] - RGB_CONFIG_BASE, rgb_config_get);
                        all_success = false;
                        break;
                    }
                }
            }
        }

        // All tests passed / 所有测试通过
        if (all_success) {
            Serial.printf(
                "Device ID[%d] RGB functionality test completed successfully / 设备ID[%d] RGB功能测试完成成功\r\n",
                device_id, device_id);
        }
    }

    return all_success;
}

/**
 * @brief Print device list information / 打印设备列表信息
 *
 * @param devices Device list pointer / 设备列表指针
 */
void printDeviceList(device_list_t *devices)
{
    if (devices == NULL) {
        Serial.println("Device list is NULL / 设备列表为空");
        return;
    }

    Serial.print("Device count / 设备数量: ");
    Serial.println(devices->count);

    for (uint8_t i = 0; i < devices->count; i++) {
        Serial.printf("Device #%d - ID: %d, Type: 0x%02X / 设备 #%d - ID: %d, 类型: 0x%02X\r\n", i,
                      devices->devices[i].id, devices->devices[i].device_type, i, devices->devices[i].id,
                      devices->devices[i].device_type);
    }
}

/**
 * @brief Update device list / 更新设备列表
 *
 * Detect connected devices and retrieve device information / 检测连接的设备并获取设备信息
 */
void updateDeviceList()
{
    // Check device connection status / 检查设备连接状态
    if (!M5Chain.isDeviceConnected()) {
        Serial.println("Devices not connected / 设备未连接");
        return;
    }

    Serial.println("Devices connected / 设备已连接");

    // Get device count / 获取设备数量
    chain_status = M5Chain.getDeviceNum(&device_nums);
    if (chain_status != CHAIN_OK) {
        Serial.printf("Get device count failed, error code: %d / 获取设备数量失败，错误码: %d\r\n", chain_status,
                      chain_status);
        return;
    }

    // Allocate device list memory / 分配设备列表内存
    devices_list = (device_list_t *)malloc(sizeof(device_list_t));
    if (!devices_list) {
        Serial.println("Memory allocation failed / 内存分配失败");
        return;
    }

    devices_list->count   = device_nums;
    devices_list->devices = (device_info_t *)malloc(sizeof(device_info_t) * device_nums);

    if (!devices_list->devices) {
        Serial.println("Device info memory allocation failed / 设备信息内存分配失败");
        free(devices_list);
        devices_list = NULL;
        return;
    }

    // Get device list / 获取设备列表
    if (M5Chain.getDeviceList(devices_list)) {
        Serial.println("Get device list success / 获取设备列表成功");
        printDeviceList(devices_list);
    } else {
        Serial.println("Get device list failed / 获取设备列表失败");
        // Clean up allocated memory / 清理已分配的内存
        free(devices_list->devices);
        free(devices_list);
        devices_list = NULL;
    }
}
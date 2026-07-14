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
#define RGB_CONFIG_DELAY_MS (10)   /**< RGB configuration delay in milliseconds / RGB配置延迟(毫秒) */

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
 * Set all channels to RGB mode / 将所有通道设置为RGB模式
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

    // Prepare RGB mode configuration / 准备RGB模式配置
    user_gpio_mode_t mode[RGB_CHANNEL_COUNT];
    for (uint8_t i = 0; i < RGB_CHANNEL_COUNT; i++) {
        mode[i] = USER_GPIO_RGB_MODE;
    }

    bool all_success = true;

    // Iterate through all devices / 遍历所有设备
    for (uint8_t i = 0; i < devices_list->count; i++) {
        // Check device type / 检查设备类型
        if (devices_list->devices[i].device_type != UNIT_8SERVOS2_CHAIN_TYPE_CODE) {
            continue;
        }

        uint8_t device_id = devices_list->devices[i].id;

        // Write operation: Set RGB mode for all channels / 写入操作：设置所有通道RGB模式
        chain_status = M5Chain.setServosModeAll(device_id, mode, RGB_CHANNEL_COUNT, &operation_status);

        // Validate write operation / 验证写入操作
        if (chain_status != CHAIN_OK) {
            Serial.printf(
                "Device ID[%d] set all channel RGB mode failed (communication error) / "
                "设备ID[%d]设置所有通道RGB模式失败(通信错误)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        if (operation_status != 1) {
            Serial.printf(
                "Device ID[%d] set all channel RGB mode failed (operation status error) / "
                "设备ID[%d]设置所有通道RGB模式失败(操作状态异常)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        Serial.printf("Device ID[%d] set all channel RGB mode success / 设备ID[%d]设置所有通道RGB模式成功\r\n",
                      device_id, device_id);
    }

    return all_success;
}

/**
 * @brief Test RGB functionality for all devices / 测试所有设备的RGB功能
 *
 * Process: Verify mode -> Set/Get color buffer -> Set/Get RGB config /
 * 流程：验证模式 -> 设置/获取颜色缓冲 -> 设置/获取RGB配置
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

        // Step 1: Verify RGB mode / 步骤1：验证RGB模式
        user_gpio_mode_t mode[RGB_CHANNEL_COUNT];
        chain_status = M5Chain.getServosModeAll(device_id, mode, RGB_CHANNEL_COUNT);

        if (chain_status != CHAIN_OK) {
            Serial.printf(
                "Device ID[%d] get all channel RGB mode failed (communication error) / "
                "设备ID[%d]获取所有通道RGB模式失败(通信错误)\r\n",
                device_id, device_id);
            all_success = false;
            break;
        }

        // Verify all channels are in RGB mode / 验证所有通道都处于RGB模式
        uint8_t err_count = 0;
        for (uint8_t j = 0; j < RGB_CHANNEL_COUNT; j++) {
            if (mode[j] != USER_GPIO_RGB_MODE) {
                err_count++;
            }
        }

        if (err_count != 0) {
            Serial.printf("Device ID[%d] has %d channels not in RGB mode / 设备ID[%d]有%d个通道不在RGB模式\r\n",
                          device_id, err_count, device_id, err_count);
            all_success = false;
            break;
        }

        // Step 2: Set and verify RGB color buffer / 步骤2：设置并验证RGB颜色缓冲
        uint32_t color[RGB_LED_COUNT];
        for (uint8_t j = 0; j < RGB_LED_COUNT; j++) {
            color[j] = test_color[test_index];
        }

        // Cycle through test colors / 循环切换测试颜色
        test_index = (test_index + 1) % RGB_COLOR_COUNT;

        // Write color buffer / 写入颜色缓冲
        chain_status = M5Chain.setServosRGBBufferAll(device_id, color, RGB_LED_COUNT);
        if (chain_status != CHAIN_OK) {
            Serial.printf(
                "Device ID[%d] set RGB color buffer failed (communication error) / "
                "设备ID[%d]设置RGB颜色缓冲失败(通信错误)\r\n",
                device_id, device_id);
            all_success = false;
            break;
        }

        // Read and verify color buffer / 读取并验证颜色缓冲
        uint32_t color_get[RGB_LED_COUNT];
        chain_status = M5Chain.getServosRGBBufferAll(device_id, color_get, RGB_LED_COUNT);
        if (chain_status != CHAIN_OK) {
            Serial.printf(
                "Device ID[%d] get RGB color buffer failed (communication error) / "
                "设备ID[%d]获取RGB颜色缓冲失败(通信错误)\r\n",
                device_id, device_id);
            all_success = false;
            break;
        }

        // Verify color buffer consistency / 验证颜色缓冲一致性
        bool color_match = true;
        for (uint8_t j = 0; j < RGB_LED_COUNT; j++) {
            if (color[j] != color_get[j]) {
                Serial.printf(
                    "Device ID[%d] LED %d color mismatch: expected 0x%06X, got 0x%06X / 设备ID[%d] LED "
                    "%d颜色不匹配：期望0x%06X，实际0x%06X\r\n",
                    device_id, j, color[j], color_get[j], device_id, j, color[j], color_get[j]);
                color_match = false;
                all_success = false;
            }
        }

        if (color_match) {
            Serial.printf(
                "Device ID[%d] RGB color buffer set and verified successfully (Color: 0x%06X) / 设备ID[%d] "
                "RGB颜色缓冲设置并验证成功(颜色: 0x%06X)\r\n",
                device_id, color[0], device_id, color[0]);
        }

        // Step 3: Test RGB configuration with brightness levels / 步骤3：测试不同亮度级别的RGB配置
        for (uint8_t j = 1; j <= RGB_CONFIG_LOOP_MAX; j++) {
            uint8_t rgb_config[RGB_CHANNEL_COUNT];

            // Prepare RGB configuration / 准备RGB配置
            for (uint8_t k = 0; k < RGB_CHANNEL_COUNT; k++) {
                rgb_config[k] = RGB_CONFIG_BASE + j;
            }

            // Write RGB configuration / 写入RGB配置
            chain_status = M5Chain.setServosRGBConfigAll(device_id, rgb_config, RGB_CHANNEL_COUNT, &operation_status);
            if (chain_status != CHAIN_OK) {
                Serial.printf(
                    "Device ID[%d] set RGB config failed (communication error) / "
                    "设备ID[%d]设置RGB配置失败(通信错误)\r\n",
                    device_id, device_id);
                all_success = false;
                break;
            }

            if (operation_status != 1) {
                Serial.printf(
                    "Device ID[%d] set RGB config failed (operation status error) / "
                    "设备ID[%d]设置RGB配置失败(操作状态异常)\r\n",
                    device_id, device_id);
                all_success = false;
            }

            // Read and verify RGB configuration / 读取并验证RGB配置
            uint8_t rgb_config_get[RGB_CHANNEL_COUNT];
            chain_status = M5Chain.getServosRGBConfigAll(device_id, rgb_config_get, RGB_CHANNEL_COUNT);
            if (chain_status != CHAIN_OK) {
                Serial.printf(
                    "Device ID[%d] get RGB config failed (communication error) / "
                    "设备ID[%d]获取RGB配置失败(通信错误)\r\n",
                    device_id, device_id);
                all_success = false;
                break;
            }

            // Verify RGB configuration consistency / 验证RGB配置一致性
            bool config_match = true;
            for (uint8_t w = 0; w < RGB_CHANNEL_COUNT; w++) {
                // Note: Config value is stored without base offset / 注意：配置值存储时不含基准偏移
                if (rgb_config_get[w] != (rgb_config[w] - RGB_CONFIG_BASE)) {
                    Serial.printf(
                        "Device ID[%d] channel %d RGB config mismatch: expected %d, got %d / 设备ID[%d]通道%d "
                        "RGB配置不匹配：期望%d，实际%d\r\n",
                        device_id, w, rgb_config[w] - RGB_CONFIG_BASE, rgb_config_get[w], device_id, w,
                        rgb_config[w] - RGB_CONFIG_BASE, rgb_config_get[w]);
                    config_match = false;
                    all_success  = false;
                    break;
                }
            }

            if (config_match && j == RGB_CONFIG_LOOP_MAX) {
                Serial.printf(
                    "Device ID[%d] RGB config test completed successfully / 设备ID[%d] RGB配置测试完成成功\r\n",
                    device_id, device_id);
            }

            delay(RGB_CONFIG_DELAY_MS);
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
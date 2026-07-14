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
#define LOOP_DELAY_MS (500) /**< Loop delay in milliseconds / 循环延迟(毫秒) */

// Global Objects and Variables / 全局对象和变量
Chain M5Chain;                          /**< M5Chain instance / M5Chain实例 */
device_list_t *devices_list = NULL;     /**< Device list pointer / 设备列表指针 */
uint16_t device_nums        = 0;        /**< Device count / 设备数量 */
uint8_t operation_status    = 0;        /**< Operation status / 操作状态 */
chain_status_t chain_status = CHAIN_OK; /**< Chain status / 链路状态 */

// Function Declarations / 函数声明
void printDeviceList(device_list_t *devices);
void updateDeviceList();
bool initializeInputModes();
bool readAndDisplayInputLevels();

void setup()
{
    Serial.begin(115200);
    Serial.println("M5Chain Unit 8Servos2 Chain Input Test");

    // Initialize M5Chain communication / 初始化M5Chain通信
    M5Chain.begin(&Serial2, 115200, RXD_PIN, TXD_PIN);

    // Update device list / 更新设备列表
    updateDeviceList();

    // Initialize all input modes with pull-down / 初始化所有输入模式并配置下拉
    if (!initializeInputModes()) {
        Serial.println("Input mode initialization failed / 输入模式初始化失败");
    }
}

void loop()
{
    // Validate device list validity / 验证设备列表有效性
    if (!devices_list) {
        Serial.println("Device list is NULL / 设备列表为空");
        delay(LOOP_DELAY_MS);
        return;
    }

    // Read and display input levels / 读取并显示输入电平
    if (!readAndDisplayInputLevels()) {
        Serial.println("Input level reading or validation failed / 输入电平读取或验证失败");
    }

    delay(LOOP_DELAY_MS);
}

/**
 * @brief Initialize input modes for all 8Servos2 devices / 初始化所有8Servos2设备的输入模式
 *
 * Set each channel individually to input mode with pull-down and verify the operation /
 * 逐个通道设置为输入模式并配置下拉，然后验证操作
 *
 * @return true if successful / 成功返回true
 * @return false if failed / 失败返回false
 */
bool initializeInputModes()
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

        // Step 1: Set input mode for each channel / 步骤1：逐个通道设置输入模式
        for (uint8_t j = 0; j < SERVOS2_GPIO_NUM_MAX; j++) {
            // Write operation: Set input mode / 写入操作：设置输入模式
            chain_status = M5Chain.setServosMode(device_id, j, USER_GPIO_INPUT_MODE, &operation_status);

            // Validate write operation / 验证写入操作
            if (chain_status != CHAIN_OK) {
                Serial.printf(
                    "Device ID[%d] channel %d set input mode failed (communication error) / "
                    "设备ID[%d]通道%d设置输入模式失败(通信错误)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                continue;
            }

            if (operation_status != 1) {
                Serial.printf(
                    "Device ID[%d] channel %d set input mode failed (operation status error) / "
                    "设备ID[%d]通道%d设置输入模式失败(操作状态异常)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                continue;
            }

            // Read verification: Confirm input mode was set correctly / 读取验证：确认输入模式设置正确
            user_gpio_mode_t read_mode;
            uint8_t read_status;

            if (M5Chain.getServosMode(device_id, j, &read_mode, &read_status) != CHAIN_OK) {
                Serial.printf("Device ID[%d] channel %d read input mode failed / 设备ID[%d]通道%d读取输入模式失败\r\n",
                              device_id, j, device_id, j);
                all_success = false;
                continue;
            }

            if (read_status != 1) {
                Serial.printf(
                    "Device ID[%d] channel %d read input mode status error / 设备ID[%d]通道%d读取输入模式状态错误\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                continue;
            }

            // Verify data consistency / 验证数据一致性
            if (read_mode != USER_GPIO_INPUT_MODE) {
                Serial.printf(
                    "Device ID[%d] channel %d input mode verification failed / 设备ID[%d]通道%d输入模式验证失败\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                continue;
            }

            Serial.printf(
                "Device ID[%d] channel %d input mode set and verified successfully / "
                "设备ID[%d]通道%d输入模式设置并验证成功\r\n",
                device_id, j, device_id, j);
        }

        // Step 2: Set pull-down for each channel / 步骤2：逐个通道设置下拉
        for (uint8_t j = 0; j < SERVOS2_GPIO_NUM_MAX; j++) {
            // Write operation: Set pull-down / 写入操作：设置下拉
            chain_status = M5Chain.setServosInputPuPd(device_id, j, USER_GPIO_PULL_DOWN, &operation_status);

            // Validate write operation / 验证写入操作
            if (chain_status != CHAIN_OK) {
                Serial.printf(
                    "Device ID[%d] channel %d set pull-down failed (communication error) / "
                    "设备ID[%d]通道%d设置下拉失败(通信错误)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                continue;
            }

            if (operation_status != 1) {
                Serial.printf(
                    "Device ID[%d] channel %d set pull-down failed (operation status error) / "
                    "设备ID[%d]通道%d设置下拉失败(操作状态异常)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                continue;
            }

            // Read verification: Confirm pull-down was set correctly / 读取验证：确认下拉设置正确
            user_gpio_pull_t read_pull;
            uint8_t read_status;

            if (M5Chain.getServosInputPuPd(device_id, j, &read_pull, &read_status) != CHAIN_OK) {
                Serial.printf("Device ID[%d] channel %d read pull-down failed / 设备ID[%d]通道%d读取下拉失败\r\n",
                              device_id, j, device_id, j);
                all_success = false;
                continue;
            }

            if (read_status != 1) {
                Serial.printf(
                    "Device ID[%d] channel %d read pull-down status error / 设备ID[%d]通道%d读取下拉状态错误\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                continue;
            }

            // Verify data consistency / 验证数据一致性
            if (read_pull != USER_GPIO_PULL_DOWN) {
                Serial.printf(
                    "Device ID[%d] channel %d pull-down verification failed / 设备ID[%d]通道%d下拉验证失败\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                continue;
            }

            Serial.printf(
                "Device ID[%d] channel %d pull-down set and verified successfully / "
                "设备ID[%d]通道%d下拉设置并验证成功\r\n",
                device_id, j, device_id, j);
        }
    }

    return all_success;
}

/**
 * @brief Read and display input levels from all channels / 读取并显示所有通道的输入电平
 *
 * Process: Verify mode -> Verify pull-down -> Read input level -> Display level /
 * 流程：验证模式 -> 验证下拉 -> 读取输入电平 -> 显示电平
 *
 * @return true if successful / 成功返回true
 * @return false if failed / 失败返回false
 */
bool readAndDisplayInputLevels()
{
    bool all_success = true;

    // Iterate through all devices / 遍历所有设备
    for (uint8_t i = 0; i < devices_list->count; i++) {
        // Check device type / 检查设备类型
        if (devices_list->devices[i].device_type != UNIT_8SERVOS2_CHAIN_TYPE_CODE) {
            continue;
        }

        uint8_t device_id = devices_list->devices[i].id;

        Serial.printf("=== Device ID[%d] Input Levels / 设备ID[%d] 输入电平 ===\r\n", device_id, device_id);

        // Process each channel / 处理每个通道
        for (uint8_t j = 0; j < SERVOS2_GPIO_NUM_MAX; j++) {
            user_gpio_mode_t mode;
            user_gpio_pull_t pull;
            user_sys_gpio_level_t level;

            // Step 1: Read and verify current mode / 步骤1：读取并验证当前模式
            if (M5Chain.getServosMode(device_id, j, &mode, &operation_status) != CHAIN_OK) {
                Serial.printf(
                    "Device ID[%d] channel %d get input mode failed (communication error) / "
                    "设备ID[%d]通道%d获取输入模式失败(通信错误)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                break;
            }

            // Validate read operation status / 验证读取操作状态
            if (operation_status != 1) {
                Serial.printf(
                    "Device ID[%d] channel %d get input mode failed (operation status error) / "
                    "设备ID[%d]通道%d获取输入模式失败(操作状态异常)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                break;
            }

            // Verify mode is correct / 验证模式正确性
            if (mode != USER_GPIO_INPUT_MODE) {
                Serial.printf(
                    "Device ID[%d] channel %d mode mismatch: expected INPUT_MODE, got %d / "
                    "设备ID[%d]通道%d模式不匹配：期望INPUT_MODE，实际%d\r\n",
                    device_id, j, mode, device_id, j, mode);
                all_success = false;
                break;
            }

            // Step 2: Read and verify pull-down configuration / 步骤2：读取并验证下拉配置
            if (M5Chain.getServosInputPuPd(device_id, j, &pull, &operation_status) != CHAIN_OK) {
                Serial.printf(
                    "Device ID[%d] channel %d get pull-down failed (communication error) / "
                    "设备ID[%d]通道%d获取下拉失败(通信错误)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                break;
            }

            // Validate read operation status / 验证读取操作状态
            if (operation_status != 1) {
                Serial.printf(
                    "Device ID[%d] channel %d get pull-down failed (operation status error) / "
                    "设备ID[%d]通道%d获取下拉失败(操作状态异常)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                break;
            }

            // Verify pull-down is correct / 验证下拉正确性
            if (pull != USER_GPIO_PULL_DOWN) {
                Serial.printf(
                    "Device ID[%d] channel %d pull-down mismatch: expected PULL_DOWN, got %d / "
                    "设备ID[%d]通道%d下拉不匹配：期望PULL_DOWN，实际%d\r\n",
                    device_id, j, pull, device_id, j, pull);
                all_success = false;
                break;
            }

            // Step 3: Read input level / 步骤3：读取输入电平
            if (M5Chain.getServosInputLevel(device_id, j, &level, &operation_status) != CHAIN_OK) {
                Serial.printf(
                    "Device ID[%d] channel %d get input level failed (communication error) / "
                    "设备ID[%d]通道%d获取输入电平失败(通信错误)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                break;
            }

            // Validate read operation status / 验证读取操作状态
            if (operation_status != 1) {
                Serial.printf(
                    "Device ID[%d] channel %d get input level failed (operation status error) / "
                    "设备ID[%d]通道%d获取输入电平失败(操作状态异常)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                break;
            }

            // Step 4: Display input level / 步骤4：显示输入电平
            const char *level_str = (level == USER_GPIO_LEVEL_HIGH) ? "HIGH" : "LOW";
            Serial.printf("  Channel %d: %s (%d) / 通道 %d: %s (%d)\r\n", j, level_str, level, j, level_str, level);
        }

        Serial.println("==========================================");
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
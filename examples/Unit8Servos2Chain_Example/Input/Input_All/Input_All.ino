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

    // Initialize all input modes with pull-up / 初始化所有输入模式并配置上拉
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
 * Set all channels to input mode with pull-up and verify the operation /
 * 将所有通道设置为输入模式并配置上拉，然后验证操作
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

    // Prepare input mode configuration / 准备输入模式配置
    user_gpio_mode_t mode[SERVOS2_GPIO_NUM_MAX];
    for (uint8_t i = 0; i < SERVOS2_GPIO_NUM_MAX; i++) {
        mode[i] = USER_GPIO_INPUT_MODE;
    }

    // Prepare pull-up configuration / 准备上拉配置
    user_gpio_pull_t pull[SERVOS2_GPIO_NUM_MAX];
    for (uint8_t i = 0; i < SERVOS2_GPIO_NUM_MAX; i++) {
        pull[i] = USER_GPIO_PULL_UP;
    }

    bool all_success = true;

    // Iterate through all devices / 遍历所有设备
    for (uint8_t i = 0; i < devices_list->count; i++) {
        // Check device type / 检查设备类型
        if (devices_list->devices[i].device_type != UNIT_8SERVOS2_CHAIN_TYPE_CODE) {
            continue;
        }

        uint8_t device_id = devices_list->devices[i].id;

        // Step 1: Write operation - Set input mode for all channels / 步骤1：写入操作 - 设置所有通道输入模式
        chain_status = M5Chain.setServosModeAll(device_id, mode, SERVOS2_GPIO_NUM_MAX, &operation_status);

        // Validate write operation / 验证写入操作
        if (chain_status != CHAIN_OK) {
            Serial.printf(
                "Device ID[%d] set all channel input mode failed (communication error) / "
                "设备ID[%d]设置所有通道输入模式失败(通信错误)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        if (operation_status != 1) {
            Serial.printf(
                "Device ID[%d] set all channel input mode failed (operation status error) / "
                "设备ID[%d]设置所有通道输入模式失败(操作状态异常)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        // Read verification: Confirm input mode was set correctly / 读取验证：确认输入模式设置正确
        user_gpio_mode_t read_mode[SERVOS2_GPIO_NUM_MAX];
        if (M5Chain.getServosModeAll(device_id, read_mode, SERVOS2_GPIO_NUM_MAX) != CHAIN_OK) {
            Serial.printf("Device ID[%d] read all channel input mode failed / 设备ID[%d]读取所有通道输入模式失败\r\n",
                          device_id, device_id);
            all_success = false;
            continue;
        }

        // Verify input mode consistency / 验证输入模式一致性
        uint8_t err_count = 0;
        for (uint8_t j = 0; j < SERVOS2_GPIO_NUM_MAX; j++) {
            if (read_mode[j] != USER_GPIO_INPUT_MODE) {
                err_count++;
                Serial.printf(
                    "Device ID[%d] channel %d mode verification failed: expected INPUT_MODE, got %d / "
                    "设备ID[%d]通道%d模式验证失败：期望INPUT_MODE，实际%d\r\n",
                    device_id, j, read_mode[j], device_id, j, read_mode[j]);
            }
        }

        if (err_count == 0) {
            Serial.printf(
                "Device ID[%d] all channel input mode set and verified successfully / "
                "设备ID[%d]所有通道输入模式设置并验证成功\r\n",
                device_id, device_id);
        } else {
            Serial.printf("Device ID[%d] has %d channels with incorrect mode / 设备ID[%d]有%d个通道模式不正确\r\n",
                          device_id, err_count, device_id, err_count);
            all_success = false;
            continue;
        }

        // Step 2: Write operation - Set pull-up for all channels / 步骤2：写入操作 - 设置所有通道上拉
        chain_status = M5Chain.setServosInputPuPdAll(device_id, pull, SERVOS2_GPIO_NUM_MAX, &operation_status);

        // Validate write operation / 验证写入操作
        if (chain_status != CHAIN_OK) {
            Serial.printf(
                "Device ID[%d] set all channel pull-up failed (communication error) / "
                "设备ID[%d]设置所有通道上拉失败(通信错误)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        if (operation_status != 1) {
            Serial.printf(
                "Device ID[%d] set all channel pull-up failed (operation status error) / "
                "设备ID[%d]设置所有通道上拉失败(操作状态异常)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        // Read verification: Confirm pull-up was set correctly / 读取验证：确认上拉设置正确
        user_gpio_pull_t read_pull[SERVOS2_GPIO_NUM_MAX];
        if (M5Chain.getServosInputPuPdAll(device_id, read_pull, SERVOS2_GPIO_NUM_MAX) != CHAIN_OK) {
            Serial.printf("Device ID[%d] read all channel pull-up failed / 设备ID[%d]读取所有通道上拉失败\r\n",
                          device_id, device_id);
            all_success = false;
            continue;
        }

        // Verify pull-up consistency / 验证上拉一致性
        err_count = 0;
        for (uint8_t j = 0; j < SERVOS2_GPIO_NUM_MAX; j++) {
            if (read_pull[j] != USER_GPIO_PULL_UP) {
                err_count++;
                Serial.printf(
                    "Device ID[%d] channel %d pull-up verification failed: expected PULL_UP, got %d / "
                    "设备ID[%d]通道%d上拉验证失败：期望PULL_UP，实际%d\r\n",
                    device_id, j, read_pull[j], device_id, j, read_pull[j]);
            }
        }

        if (err_count == 0) {
            Serial.printf(
                "Device ID[%d] all channel pull-up set and verified successfully / "
                "设备ID[%d]所有通道上拉设置并验证成功\r\n",
                device_id, device_id);
        } else {
            Serial.printf("Device ID[%d] has %d channels with incorrect pull-up / 设备ID[%d]有%d个通道上拉不正确\r\n",
                          device_id, err_count, device_id, err_count);
            all_success = false;
        }
    }

    return all_success;
}

/**
 * @brief Read and display input levels from all channels / 读取并显示所有通道的输入电平
 *
 * Process: Verify mode -> Verify pull-up -> Read input levels -> Display levels /
 * 流程：验证模式 -> 验证上拉 -> 读取输入电平 -> 显示电平
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
        user_gpio_mode_t mode[SERVOS2_GPIO_NUM_MAX];
        user_gpio_pull_t pull[SERVOS2_GPIO_NUM_MAX];
        user_sys_gpio_level_t state[SERVOS2_GPIO_NUM_MAX];

        // Step 1: Read and verify current mode / 步骤1：读取并验证当前模式
        if (M5Chain.getServosModeAll(device_id, mode, SERVOS2_GPIO_NUM_MAX) != CHAIN_OK) {
            Serial.printf(
                "Device ID[%d] get all channel input mode failed (communication error) / "
                "设备ID[%d]获取所有通道输入模式失败(通信错误)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        // Verify all channels are in input mode / 验证所有通道都处于输入模式
        uint8_t err_count = 0;
        for (uint8_t j = 0; j < SERVOS2_GPIO_NUM_MAX; j++) {
            if (mode[j] != USER_GPIO_INPUT_MODE) {
                err_count++;
            }
        }

        if (err_count != 0) {
            Serial.printf("Device ID[%d] has %d channels not in input mode / 设备ID[%d]有%d个通道不在输入模式\r\n",
                          device_id, err_count, device_id, err_count);
            all_success = false;
            continue;
        }

        // Step 2: Read and verify pull-up configuration / 步骤2：读取并验证上拉配置
        if (M5Chain.getServosInputPuPdAll(device_id, pull, SERVOS2_GPIO_NUM_MAX) != CHAIN_OK) {
            Serial.printf(
                "Device ID[%d] get all channel pull-up failed (communication error) / "
                "设备ID[%d]获取所有通道上拉失败(通信错误)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        // Verify all channels have pull-up enabled / 验证所有通道都启用了上拉
        err_count = 0;
        for (uint8_t j = 0; j < SERVOS2_GPIO_NUM_MAX; j++) {
            if (pull[j] != USER_GPIO_PULL_UP) {
                err_count++;
            }
        }

        if (err_count != 0) {
            Serial.printf("Device ID[%d] has %d channels without pull-up / 设备ID[%d]有%d个通道未启用上拉\r\n",
                          device_id, err_count, device_id, err_count);
            all_success = false;
            continue;
        }

        // Step 3: Read input levels from all channels / 步骤3：读取所有通道的输入电平
        if (M5Chain.getServosInputLevelAll(device_id, state, SERVOS2_GPIO_NUM_MAX) != CHAIN_OK) {
            Serial.printf(
                "Device ID[%d] get all channel input level failed (communication error) / "
                "设备ID[%d]获取所有通道输入电平失败(通信错误)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        // Step 4: Display input levels for all channels / 步骤4：显示所有通道的输入电平
        Serial.printf("=== Device ID[%d] Input Levels / 设备ID[%d] 输入电平 ===\r\n", device_id, device_id);
        for (uint8_t j = 0; j < SERVOS2_GPIO_NUM_MAX; j++) {
            const char *level_str = (state[j] == USER_GPIO_LEVEL_HIGH) ? "HIGH" : "LOW";
            Serial.printf("  Channel %d: %s (%d) /  通道 %d: %s (%d)\r\n", j, level_str, state[j], j, level_str,
                          state[j]);
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
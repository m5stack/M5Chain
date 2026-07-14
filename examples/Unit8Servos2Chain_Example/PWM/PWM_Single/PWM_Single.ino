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
#define LOOP_DELAY_MS     (1000) /**< Loop delay in milliseconds / 循环延迟(毫秒) */
#define PWM_CHANNEL_COUNT (8)    /**< Number of PWM channels / PWM通道数量 */
#define PWM_TIMER_COUNT   (2)    /**< Number of PWM timers / PWM定时器数量 */

// Global Objects and Variables / 全局对象和变量
Chain M5Chain;                          /**< M5Chain instance / M5Chain实例 */
device_list_t *devices_list = NULL;     /**< Device list pointer / 设备列表指针 */
uint16_t device_nums        = 0;        /**< Device count / 设备数量 */
uint8_t operation_status    = 0;        /**< Operation status / 操作状态 */
chain_status_t chain_status = CHAIN_OK; /**< Chain status / 链路状态 */

// PWM Configuration / PWM配置
uint16_t freq[PWM_TIMER_COUNT]  = {3000, 6000}; /**< PWM frequencies in Hz / PWM频率(Hz) */
uint8_t duty[PWM_CHANNEL_COUNT] = {10, 20, 30, 40,
                                   50, 60, 70, 80}; /**< PWM duty cycles in percentage / PWM占空比(百分比) */

// Function Declarations / 函数声明
void printDeviceList(device_list_t *devices);
void updateDeviceList();
bool initializePwmConfiguration();
bool verifyPwmConfiguration();

void setup()
{
    Serial.begin(115200);
    Serial.println("M5Chain Unit 8Servos2 Chain Test");

    // Initialize M5Chain communication / 初始化M5Chain通信
    M5Chain.begin(&Serial2, 115200, RXD_PIN, TXD_PIN);

    // Update device list / 更新设备列表
    updateDeviceList();

    // Initialize PWM configuration / 初始化PWM配置
    if (!initializePwmConfiguration()) {
        Serial.println("PWM configuration initialization failed / PWM配置初始化失败");
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

    // Verify PWM configuration / 验证PWM配置
    if (!verifyPwmConfiguration()) {
        Serial.println("PWM configuration verification failed / PWM配置验证失败");
    }

    delay(LOOP_DELAY_MS);
}

/**
 * @brief Initialize PWM configuration for all 8Servos2 devices / 初始化所有8Servos2设备的PWM配置
 *
 * Process: Set PWM mode -> Set timer config -> Set duty cycles /
 * 流程：设置PWM模式 -> 设置定时器配置 -> 设置占空比
 *
 * @return true if successful / 成功返回true
 * @return false if failed / 失败返回false
 */
bool initializePwmConfiguration()
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

        // Step 1: Set PWM mode for all channels / 步骤1：设置所有通道PWM模式
        for (uint8_t j = 0; j < PWM_CHANNEL_COUNT; j++) {
            chain_status = M5Chain.setServosMode(device_id, j, USER_GPIO_PWM_MODE, &operation_status);

            if (chain_status != CHAIN_OK) {
                Serial.printf(
                    "Device ID[%d] channel %d set PWM mode failed (communication error) / "
                    "设备ID[%d]通道%d设置PWM模式失败(通信错误)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                continue;
            }

            if (operation_status != 1) {
                Serial.printf(
                    "Device ID[%d] channel %d set PWM mode failed (operation status error) / "
                    "设备ID[%d]通道%d设置PWM模式失败(操作状态异常)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                continue;
            }

            Serial.printf("Device ID[%d] channel %d set PWM mode success / 设备ID[%d]通道%d设置PWM模式成功\r\n",
                          device_id, j, device_id, j);
        }

        // Step 2: Set timer configuration / 步骤2：设置定时器配置
        for (uint8_t j = 0; j < PWM_TIMER_COUNT; j++) {
            chain_status = M5Chain.setServosTimeConfig(device_id, j, freq[j], &operation_status);

            if (chain_status != CHAIN_OK) {
                Serial.printf(
                    "Device ID[%d] timer %d set frequency failed (communication error) / "
                    "设备ID[%d]定时器%d设置频率失败(通信错误)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                continue;
            }

            if (operation_status != 1) {
                Serial.printf(
                    "Device ID[%d] timer %d set frequency failed (operation status error) / "
                    "设备ID[%d]定时器%d设置频率失败(操作状态异常)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                continue;
            }

            Serial.printf(
                "Device ID[%d] timer %d set frequency success (%d Hz) / 设备ID[%d]定时器%d设置频率成功(%d Hz)\r\n",
                device_id, j, freq[j], device_id, j, freq[j]);
        }

        // Step 3: Set PWM duty cycles / 步骤3：设置PWM占空比
        for (uint8_t j = 0; j < PWM_CHANNEL_COUNT; j++) {
            chain_status = M5Chain.setServosPwmDuty(device_id, j, duty[j], &operation_status);

            if (chain_status != CHAIN_OK) {
                Serial.printf(
                    "Device ID[%d] channel %d set PWM duty failed (communication error) / "
                    "设备ID[%d]通道%d设置PWM占空比失败(通信错误)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                break;
            }

            if (operation_status != 1) {
                Serial.printf(
                    "Device ID[%d] channel %d set PWM duty failed (operation status error) / "
                    "设备ID[%d]通道%d设置PWM占空比失败(操作状态异常)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                break;
            }

            Serial.printf(
                "Device ID[%d] channel %d set PWM duty success (%d%%) / 设备ID[%d]通道%d设置PWM占空比成功(%d%%)\r\n",
                device_id, j, duty[j], device_id, j, duty[j]);
        }
    }

    return all_success;
}

/**
 * @brief Verify PWM configuration for all channels / 验证所有通道的PWM配置
 *
 * Process: Verify mode -> Verify timer config -> Verify duty cycles /
 * 流程：验证模式 -> 验证定时器配置 -> 验证占空比
 *
 * @return true if successful / 成功返回true
 * @return false if failed / 失败返回false
 */
bool verifyPwmConfiguration()
{
    bool all_success = true;

    // Iterate through all devices / 遍历所有设备
    for (uint8_t i = 0; i < devices_list->count; i++) {
        // Check device type / 检查设备类型
        if (devices_list->devices[i].device_type != UNIT_8SERVOS2_CHAIN_TYPE_CODE) {
            continue;
        }

        uint8_t device_id = devices_list->devices[i].id;

        // Step 1: Verify PWM mode for all channels / 步骤1：验证所有通道PWM模式
        for (uint8_t j = 0; j < PWM_CHANNEL_COUNT; j++) {
            user_gpio_mode_t mode;

            chain_status = M5Chain.getServosMode(device_id, j, &mode, &operation_status);

            if (chain_status != CHAIN_OK) {
                Serial.printf(
                    "Device ID[%d] channel %d get PWM mode failed (communication error) / "
                    "设备ID[%d]通道%d获取PWM模式失败(通信错误)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                break;
            }

            if (operation_status != 1) {
                Serial.printf(
                    "Device ID[%d] channel %d get PWM mode failed (operation status error) / "
                    "设备ID[%d]通道%d获取PWM模式失败(操作状态异常)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                break;
            }

            if (mode != USER_GPIO_PWM_MODE) {
                Serial.printf("Device ID[%d] channel %d PWM mode mismatch / 设备ID[%d]通道%d PWM模式不匹配\r\n",
                              device_id, j, device_id, j);
                all_success = false;
                break;
            }
        }

        // Step 2: Verify timer configuration / 步骤2：验证定时器配置
        for (uint8_t j = 0; j < PWM_TIMER_COUNT; j++) {
            uint16_t freq_temp = 0;

            chain_status = M5Chain.getServosTimeConfig(device_id, j, &freq_temp, &operation_status);

            if (chain_status != CHAIN_OK) {
                Serial.printf(
                    "Device ID[%d] timer %d get frequency failed (communication error) / "
                    "设备ID[%d]定时器%d获取频率失败(通信错误)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                break;
            }

            if (operation_status != 1) {
                Serial.printf(
                    "Device ID[%d] timer %d get frequency failed (operation status error) / "
                    "设备ID[%d]定时器%d获取频率失败(操作状态异常)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                break;
            }

            if (freq_temp != freq[j]) {
                Serial.printf(
                    "Device ID[%d] timer %d frequency mismatch: expected %d Hz, got %d Hz / "
                    "设备ID[%d]定时器%d频率不匹配：期望%d Hz，实际%d Hz\r\n",
                    device_id, j, freq[j], freq_temp, device_id, j, freq[j], freq_temp);
                all_success = false;
                break;
            }
        }

        // Step 3: Verify PWM duty cycles / 步骤3：验证PWM占空比
        for (uint8_t j = 0; j < PWM_CHANNEL_COUNT; j++) {
            uint8_t duty_temp = 0;

            chain_status = M5Chain.getServosPwmDuty(device_id, j, &duty_temp, &operation_status);

            if (chain_status != CHAIN_OK) {
                Serial.printf(
                    "Device ID[%d] channel %d get PWM duty failed (communication error) / "
                    "设备ID[%d]通道%d获取PWM占空比失败(通信错误)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                break;
            }

            if (operation_status != 1) {
                Serial.printf(
                    "Device ID[%d] channel %d get PWM duty failed (operation status error) / "
                    "设备ID[%d]通道%d获取PWM占空比失败(操作状态异常)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                break;
            }

            if (duty_temp != duty[j]) {
                Serial.printf(
                    "Device ID[%d] channel %d duty mismatch: expected %d%%, got %d%% / "
                    "设备ID[%d]通道%d占空比不匹配：期望%d%%，实际%d%%\r\n",
                    device_id, j, duty[j], duty_temp, device_id, j, duty[j], duty_temp);
                all_success = false;
                break;
            }
        }

        // All verifications passed / 所有验证通过
        if (all_success) {
            Serial.printf("Device ID[%d] PWM configuration verified successfully / 设备ID[%d] PWM配置验证成功\r\n",
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
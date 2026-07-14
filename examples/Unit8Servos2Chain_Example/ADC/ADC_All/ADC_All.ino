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
bool initializeAdcModes();
bool readAndDisplayAdcValues();

void setup()
{
    Serial.begin(115200);
    Serial.println("M5Chain Unit 8Servos2 Chain ADC Test");

    // Initialize M5Chain communication / 初始化M5Chain通信
    M5Chain.begin(&Serial2, 115200, RXD_PIN, TXD_PIN);

    // Update device list / 更新设备列表
    updateDeviceList();

    // Initialize all ADC modes / 初始化所有ADC模式
    if (!initializeAdcModes()) {
        Serial.println("ADC mode initialization failed / ADC模式初始化失败");
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

    // Read and display ADC values / 读取并显示ADC值
    if (!readAndDisplayAdcValues()) {
        Serial.println("ADC reading or validation failed / ADC读取或验证失败");
    }

    delay(LOOP_DELAY_MS);
}

/**
 * @brief Initialize ADC modes for all 8Servos2 devices / 初始化所有8Servos2设备的ADC模式
 *
 * Set all channels to ADC mode and verify the operation / 将所有通道设置为ADC模式并验证操作
 *
 * @return true if successful / 成功返回true
 * @return false if failed / 失败返回false
 */
bool initializeAdcModes()
{
    if (!devices_list) {
        Serial.println("Device list is NULL, cannot initialize / 设备列表为空，无法初始化");
        return false;
    }

    // Prepare ADC mode configuration / 准备ADC模式配置
    user_gpio_mode_t mode[SERVOS2_GPIO_NUM_MAX];
    for (uint8_t i = 0; i < SERVOS2_GPIO_NUM_MAX; i++) {
        mode[i] = USER_GPIO_ADC_MODE;
    }

    bool all_success = true;

    // Iterate through all devices / 遍历所有设备
    for (uint8_t i = 0; i < devices_list->count; i++) {
        // Check device type / 检查设备类型
        if (devices_list->devices[i].device_type != UNIT_8SERVOS2_CHAIN_TYPE_CODE) {
            continue;
        }

        uint8_t device_id = devices_list->devices[i].id;

        // Write operation: Set ADC mode for all channels / 写入操作：设置所有通道ADC模式
        chain_status = M5Chain.setServosModeAll(device_id, mode, SERVOS2_GPIO_NUM_MAX, &operation_status);

        // Validate write operation / 验证写入操作
        if (chain_status != CHAIN_OK) {
            Serial.printf(
                "Device ID[%d] set all channel ADC mode failed (communication error) / "
                "设备ID[%d]设置所有通道ADC模式失败(通信错误)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        if (operation_status != 1) {
            Serial.printf(
                "Device ID[%d] set all channel ADC mode failed (operation status error) / "
                "设备ID[%d]设置所有通道ADC模式失败(操作状态异常)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        // Read verification: Confirm mode was set correctly / 读取验证：确认模式设置正确
        user_gpio_mode_t read_mode[SERVOS2_GPIO_NUM_MAX];
        if (M5Chain.getServosModeAll(device_id, read_mode, SERVOS2_GPIO_NUM_MAX) != CHAIN_OK) {
            Serial.printf("Device ID[%d] read all channel ADC mode failed / 设备ID[%d]读取所有通道ADC模式失败\r\n",
                          device_id, device_id);
            all_success = false;
            continue;
        }

        // Verify data consistency for all channels / 验证所有通道的数据一致性
        uint8_t err_count = 0;
        for (uint8_t j = 0; j < SERVOS2_GPIO_NUM_MAX; j++) {
            if (read_mode[j] != USER_GPIO_ADC_MODE) {
                err_count++;
                Serial.printf(
                    "Device ID[%d] channel %d mode verification failed: expected ADC_MODE, got %d / "
                    "设备ID[%d]通道%d模式验证失败：期望ADC_MODE，实际%d\r\n",
                    device_id, j, read_mode[j], device_id, j, read_mode[j]);
            }
        }

        if (err_count == 0) {
            Serial.printf(
                "Device ID[%d] all channel ADC mode set and verified successfully / "
                "设备ID[%d]所有通道ADC模式设置并验证成功\r\n",
                device_id, device_id);
        } else {
            Serial.printf("Device ID[%d] has %d channels with incorrect mode / 设备ID[%d]有%d个通道模式不正确\r\n",
                          device_id, err_count, device_id, err_count);
            all_success = false;
        }
    }

    return all_success;
}

/**
 * @brief Read and display ADC values from all channels / 读取并显示所有通道的ADC值
 *
 * Process: Verify mode -> Read ADC values -> Display values /
 * 流程：验证模式 -> 读取ADC值 -> 显示数值
 *
 * @return true if successful / 成功返回true
 * @return false if failed / 失败返回false
 */
bool readAndDisplayAdcValues()
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
        uint16_t adc_val[SERVOS2_GPIO_NUM_MAX];

        // Step 1: Read and verify current mode / 步骤1：读取并验证当前模式
        if (M5Chain.getServosModeAll(device_id, mode, SERVOS2_GPIO_NUM_MAX) != CHAIN_OK) {
            Serial.printf(
                "Device ID[%d] get all channel ADC mode failed (communication error) / "
                "设备ID[%d]获取所有通道ADC模式失败(通信错误)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        // Verify all channels are in ADC mode / 验证所有通道都处于ADC模式
        uint8_t err_count = 0;
        for (uint8_t j = 0; j < SERVOS2_GPIO_NUM_MAX; j++) {
            if (mode[j] != USER_GPIO_ADC_MODE) {
                err_count++;
            }
        }

        if (err_count != 0) {
            Serial.printf("Device ID[%d] has %d channels not in ADC mode / 设备ID[%d]有%d个通道不在ADC模式\r\n",
                          device_id, err_count, device_id, err_count);
            all_success = false;
            continue;
        }

        // Step 2: Read ADC values from all channels / 步骤2：读取所有通道的ADC值
        if (M5Chain.getServosAdcValueAll(device_id, adc_val, SERVOS2_GPIO_NUM_MAX) != CHAIN_OK) {
            Serial.printf(
                "Device ID[%d] get all channel ADC value failed (communication error) / "
                "设备ID[%d]获取所有通道ADC值失败(通信错误)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        // Step 3: Display ADC values for all channels / 步骤3：显示所有通道的ADC值
        Serial.printf("=== Device ID[%d] ADC Values / 设备ID[%d] ADC值 ===\r\n", device_id, device_id);
        for (uint8_t j = 0; j < SERVOS2_GPIO_NUM_MAX; j++) {
            Serial.printf("  Channel %d: %d (0x%04X) / 通道 %d: %d (0x%04X)\r\n", j, adc_val[j], adc_val[j], j,
                          adc_val[j], adc_val[j]);
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
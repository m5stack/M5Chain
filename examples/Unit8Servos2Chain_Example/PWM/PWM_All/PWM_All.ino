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
#define LOOP_DELAY_MS   (500) /**< Loop delay in milliseconds / 循环延迟(毫秒) */
#define PWM_TIMER_COUNT (2)   /**< Number of PWM timers / PWM定时器数量 */

// Global Objects and Variables / 全局对象和变量
Chain M5Chain;                          /**< M5Chain instance / M5Chain实例 */
device_list_t *devices_list = NULL;     /**< Device list pointer / 设备列表指针 */
uint16_t device_nums        = 0;        /**< Device count / 设备数量 */
uint8_t operation_status    = 0;        /**< Operation status / 操作状态 */
chain_status_t chain_status = CHAIN_OK; /**< Chain status / 链路状态 */

// PWM Configuration / PWM配置
uint16_t freq[PWM_TIMER_COUNT]     = {5000, 10000}; /**< PWM frequencies in Hz / PWM频率(Hz) */
uint8_t duty[SERVOS2_GPIO_NUM_MAX] = {10, 20, 30, 40,
                                      50, 60, 70, 80}; /**< PWM duty cycles in percentage / PWM占空比(百分比) */

// Function Declarations / 函数声明
void printDeviceList(device_list_t *devices);
void updateDeviceList();
bool initializePwmModes();
bool verifyPwmConfiguration();

void setup()
{
    Serial.begin(115200);
    Serial.println("M5Chain Unit 8Servos2 Chain PWM Test");

    // Initialize M5Chain communication / 初始化M5Chain通信
    M5Chain.begin(&Serial2, 115200, RXD_PIN, TXD_PIN);

    // Update device list / 更新设备列表
    updateDeviceList();

    // Initialize all PWM modes and configurations / 初始化所有PWM模式和配置
    if (!initializePwmModes()) {
        Serial.println("PWM mode initialization failed / PWM模式初始化失败");
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
 * @brief Initialize PWM modes for all 8Servos2 devices / 初始化所有8Servos2设备的PWM模式
 *
 * Set all channels to PWM mode, configure timer frequencies and duty cycles /
 * 将所有通道设置为PWM模式，配置定时器频率和占空比
 *
 * @return true if successful / 成功返回true
 * @return false if failed / 失败返回false
 */
bool initializePwmModes()
{
    if (!devices_list) {
        Serial.println("Device list is NULL, cannot initialize / 设备列表为空，无法初始化");
        return false;
    }

    // Prepare PWM mode configuration / 准备PWM模式配置
    user_gpio_mode_t mode[SERVOS2_GPIO_NUM_MAX];
    for (uint8_t i = 0; i < SERVOS2_GPIO_NUM_MAX; i++) {
        mode[i] = USER_GPIO_PWM_MODE;
    }

    bool all_success = true;

    // Iterate through all devices / 遍历所有设备
    for (uint8_t i = 0; i < devices_list->count; i++) {
        // Check device type / 检查设备类型
        if (devices_list->devices[i].device_type != UNIT_8SERVOS2_CHAIN_TYPE_CODE) {
            continue;
        }

        uint8_t device_id = devices_list->devices[i].id;

        // Step 1: Write operation - Set PWM mode for all channels / 步骤1：写入操作 - 设置所有通道PWM模式
        chain_status = M5Chain.setServosModeAll(device_id, mode, SERVOS2_GPIO_NUM_MAX, &operation_status);

        // Validate write operation / 验证写入操作
        if (chain_status != CHAIN_OK) {
            Serial.printf(
                "Device ID[%d] set all channel PWM mode failed (communication error) / "
                "设备ID[%d]设置所有通道PWM模式失败(通信错误)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        if (operation_status != 1) {
            Serial.printf(
                "Device ID[%d] set all channel PWM mode failed (operation status error) / "
                "设备ID[%d]设置所有通道PWM模式失败(操作状态异常)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        // Read verification: Confirm PWM mode was set correctly / 读取验证：确认PWM模式设置正确
        user_gpio_mode_t read_mode[SERVOS2_GPIO_NUM_MAX];
        if (M5Chain.getServosModeAll(device_id, read_mode, SERVOS2_GPIO_NUM_MAX) != CHAIN_OK) {
            Serial.printf("Device ID[%d] read all channel PWM mode failed / 设备ID[%d]读取所有通道PWM模式失败\r\n",
                          device_id, device_id);
            all_success = false;
            continue;
        }

        // Verify PWM mode consistency / 验证PWM模式一致性
        uint8_t err_count = 0;
        for (uint8_t j = 0; j < SERVOS2_GPIO_NUM_MAX; j++) {
            if (read_mode[j] != USER_GPIO_PWM_MODE) {
                err_count++;
                Serial.printf(
                    "Device ID[%d] channel %d mode verification failed: expected PWM_MODE, got %d / "
                    "设备ID[%d]通道%d模式验证失败：期望PWM_MODE，实际%d\r\n",
                    device_id, j, read_mode[j], device_id, j, read_mode[j]);
            }
        }

        if (err_count == 0) {
            Serial.printf(
                "Device ID[%d] all channel PWM mode set and verified successfully / "
                "设备ID[%d]所有通道PWM模式设置并验证成功\r\n",
                device_id, device_id);
        } else {
            Serial.printf("Device ID[%d] has %d channels with incorrect mode / 设备ID[%d]有%d个通道模式不正确\r\n",
                          device_id, err_count, device_id, err_count);
            all_success = false;
            continue;
        }

        // Step 2: Write operation - Set timer frequencies / 步骤2：写入操作 - 设置定时器频率
        chain_status = M5Chain.setServosTimeConfigAll(device_id, freq, PWM_TIMER_COUNT, &operation_status);

        // Validate write operation / 验证写入操作
        if (chain_status != CHAIN_OK) {
            Serial.printf(
                "Device ID[%d] set timer config failed (communication error) / "
                "设备ID[%d]设置定时器配置失败(通信错误)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        if (operation_status != 1) {
            Serial.printf(
                "Device ID[%d] set timer config failed (operation status error) / "
                "设备ID[%d]设置定时器配置失败(操作状态异常)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        // Read verification: Confirm timer config was set correctly / 读取验证：确认定时器配置设置正确
        uint16_t read_freq[PWM_TIMER_COUNT];
        if (M5Chain.getServosTimeConfigAll(device_id, read_freq, PWM_TIMER_COUNT) != CHAIN_OK) {
            Serial.printf("Device ID[%d] read timer config failed / 设备ID[%d]读取定时器配置失败\r\n", device_id,
                          device_id);
            all_success = false;
            continue;
        }

        // Verify timer config consistency / 验证定时器配置一致性
        err_count = 0;
        for (uint8_t j = 0; j < PWM_TIMER_COUNT; j++) {
            if (read_freq[j] != freq[j]) {
                err_count++;
                Serial.printf(
                    "Device ID[%d] timer %d frequency mismatch: expected %d Hz, got %d Hz / "
                    "设备ID[%d]定时器%d频率不匹配：期望%d Hz，实际%d Hz\r\n",
                    device_id, j, freq[j], read_freq[j], device_id, j, freq[j], read_freq[j]);
            }
        }

        if (err_count == 0) {
            Serial.printf(
                "Device ID[%d] timer config set and verified successfully (Timer0: %d Hz, Timer1: %d Hz) / "
                "设备ID[%d]定时器配置设置并验证成功(定时器0: %d Hz, 定时器1: %d Hz)\r\n",
                device_id, freq[0], freq[1], device_id, freq[0], freq[1]);
        } else {
            Serial.printf("Device ID[%d] has %d timers with incorrect frequency / 设备ID[%d]有%d个定时器频率不正确\r\n",
                          device_id, err_count, device_id, err_count);
            all_success = false;
            continue;
        }

        // Step 3: Write operation - Set PWM duty cycles / 步骤3：写入操作 - 设置PWM占空比
        chain_status = M5Chain.setServosPwmDutyAll(device_id, duty, SERVOS2_GPIO_NUM_MAX, &operation_status);

        // Validate write operation / 验证写入操作
        if (chain_status != CHAIN_OK) {
            Serial.printf(
                "Device ID[%d] set all channel PWM duty failed (communication error) / "
                "设备ID[%d]设置所有通道PWM占空比失败(通信错误)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        if (operation_status != 1) {
            Serial.printf(
                "Device ID[%d] set all channel PWM duty failed (operation status error) / "
                "设备ID[%d]设置所有通道PWM占空比失败(操作状态异常)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        // Read verification: Confirm PWM duty was set correctly / 读取验证：确认PWM占空比设置正确
        uint8_t read_duty[SERVOS2_GPIO_NUM_MAX];
        if (M5Chain.getServosPwmDutyAll(device_id, read_duty, SERVOS2_GPIO_NUM_MAX) != CHAIN_OK) {
            Serial.printf("Device ID[%d] read all channel PWM duty failed / 设备ID[%d]读取所有通道PWM占空比失败\r\n",
                          device_id, device_id);
            all_success = false;
            continue;
        }

        // Verify PWM duty consistency / 验证PWM占空比一致性
        err_count = 0;
        for (uint8_t j = 0; j < SERVOS2_GPIO_NUM_MAX; j++) {
            if (read_duty[j] != duty[j]) {
                err_count++;
                Serial.printf(
                    "Device ID[%d] channel %d duty mismatch: expected %d%%, got %d%% / "
                    "设备ID[%d]通道%d占空比不匹配：期望%d%%，实际%d%%\r\n",
                    device_id, j, duty[j], read_duty[j], device_id, j, duty[j], read_duty[j]);
            }
        }

        if (err_count == 0) {
            Serial.printf(
                "Device ID[%d] all channel PWM duty set and verified successfully / "
                "设备ID[%d]所有通道PWM占空比设置并验证成功\r\n",
                device_id, device_id);
            Serial.printf("  Duty cycles: ", device_id);
            for (uint8_t j = 0; j < SERVOS2_GPIO_NUM_MAX; j++) {
                Serial.printf("Ch%d=%d%% ", j, duty[j]);
            }
            Serial.println();
        } else {
            Serial.printf("Device ID[%d] has %d channels with incorrect duty / 设备ID[%d]有%d个通道占空比不正确\r\n",
                          device_id, err_count, device_id, err_count);
            all_success = false;
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
        user_gpio_mode_t mode[SERVOS2_GPIO_NUM_MAX];
        uint16_t freq_temp[PWM_TIMER_COUNT];
        uint8_t duty_temp[SERVOS2_GPIO_NUM_MAX];

        // Step 1: Verify PWM mode / 步骤1：验证PWM模式
        if (M5Chain.getServosModeAll(device_id, mode, SERVOS2_GPIO_NUM_MAX) != CHAIN_OK) {
            Serial.printf(
                "Device ID[%d] get all channel PWM mode failed (communication error) / "
                "设备ID[%d]获取所有通道PWM模式失败(通信错误)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        uint8_t err_count = 0;
        for (uint8_t j = 0; j < SERVOS2_GPIO_NUM_MAX; j++) {
            if (mode[j] != USER_GPIO_PWM_MODE) {
                err_count++;
            }
        }

        if (err_count != 0) {
            Serial.printf("Device ID[%d] has %d channels not in PWM mode / 设备ID[%d]有%d个通道不在PWM模式\r\n",
                          device_id, err_count, device_id, err_count);
            all_success = false;
            continue;
        }

        // Step 2: Verify timer configuration / 步骤2：验证定时器配置
        if (M5Chain.getServosTimeConfigAll(device_id, freq_temp, PWM_TIMER_COUNT) != CHAIN_OK) {
            Serial.printf(
                "Device ID[%d] get timer config failed (communication error) / "
                "设备ID[%d]获取定时器配置失败(通信错误)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        err_count = 0;
        for (uint8_t j = 0; j < PWM_TIMER_COUNT; j++) {
            if (freq_temp[j] != freq[j]) {
                err_count++;
            }
        }

        if (err_count != 0) {
            Serial.printf("Device ID[%d] has %d timers with incorrect frequency / 设备ID[%d]有%d个定时器频率不正确\r\n",
                          device_id, err_count, device_id, err_count);
            all_success = false;
            continue;
        }

        // Step 3: Verify PWM duty cycles / 步骤3：验证PWM占空比
        if (M5Chain.getServosPwmDutyAll(device_id, duty_temp, SERVOS2_GPIO_NUM_MAX) != CHAIN_OK) {
            Serial.printf(
                "Device ID[%d] get all channel PWM duty failed (communication error) / "
                "设备ID[%d]获取所有通道PWM占空比失败(通信错误)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        err_count = 0;
        for (uint8_t j = 0; j < SERVOS2_GPIO_NUM_MAX; j++) {
            if (duty_temp[j] != duty[j]) {
                err_count++;
            }
        }

        if (err_count != 0) {
            Serial.printf("Device ID[%d] has %d channels with incorrect duty / 设备ID[%d]有%d个通道占空比不正确\r\n",
                          device_id, err_count, device_id, err_count);
            all_success = false;
            continue;
        }

        // All verifications passed / 所有验证通过
        Serial.printf("Device ID[%d] PWM configuration verified successfully / 设备ID[%d] PWM配置验证成功\r\n",
                      device_id, device_id);
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
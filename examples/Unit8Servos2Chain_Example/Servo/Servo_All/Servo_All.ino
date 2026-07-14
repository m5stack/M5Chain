/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#include "M5Chain.h"

// Serial Pin Definitions / 串口引脚定义
#define TXD_PIN (GPIO_NUM_32)  // Transmit Pin / 发送引脚
#define RXD_PIN (GPIO_NUM_33)  // Receive Pin / 接收引脚

// Servo Configuration Constants / 舵机配置常量
#define ANGLE_INCREMENT (20)   // Angle increment / 角度增量
#define MAX_ANGLE       (180)  // Maximum angle / 最大角度
#define LOOP_DELAY_MS   (100)  // Loop delay in milliseconds / 循环延迟(毫秒)

// Global Objects and Variables / 全局对象和变量
Chain M5Chain;
device_list_t *devices_list = NULL;  // Device list pointer / 设备列表指针
uint16_t device_nums        = 0;     // Device count / 设备数量
uint8_t operation_status    = 0;     // Operation status / 操作状态
chain_status_t chain_status = CHAIN_OK;
uint8_t angle               = 0;  // Current angle / 当前角度

// Function Declarations / 函数声明
void printDeviceList(device_list_t *devices);
void updateDeviceList();
bool initializeServoModes();
bool validateAndSetServoAngles();

void setup()
{
    Serial.begin(115200);
    Serial.println("M5Chain Unit 8Servos2 Chain Test");

    // Initialize M5Chain communication / 初始化M5Chain通信
    M5Chain.begin(&Serial2, 115200, RXD_PIN, TXD_PIN);

    // Update device list / 更新设备列表
    updateDeviceList();

    // Initialize all servo modes / 初始化所有舵机模式
    if (!initializeServoModes()) {
        Serial.println("Servo mode initialization failed / 舵机模式初始化失败");
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

    // Execute servo angle setting and validation / 执行舵机角度设置和验证
    if (!validateAndSetServoAngles()) {
        Serial.println("Servo angle setting or validation failed / 舵机角度设置或验证失败");
    }

    delay(LOOP_DELAY_MS);
}

/**
 * Initialize servo modes for all 8Servos2 devices / 初始化所有8Servos2设备的舵机模式
 * @return Success status / 是否成功
 */
bool initializeServoModes()
{
    if (!devices_list) {
        Serial.println("Device list is NULL, cannot initialize / 设备列表为空，无法初始化");
        return false;
    }

    // Prepare servo mode configuration / 准备舵机模式配置
    user_gpio_mode_t mode[SERVOS2_GPIO_NUM_MAX];
    for (uint8_t i = 0; i < SERVOS2_GPIO_NUM_MAX; i++) {
        mode[i] = USER_GPIO_SERVO_MODE;
    }

    bool all_success = true;

    // Iterate through all devices / 遍历所有设备
    for (uint8_t i = 0; i < devices_list->count; i++) {
        // Check device type / 检查设备类型
        if (devices_list->devices[i].device_type != UNIT_8SERVOS2_CHAIN_TYPE_CODE) {
            continue;
        }

        uint8_t device_id = devices_list->devices[i].id;

        // Set servo mode / 设置舵机模式
        chain_status = M5Chain.setServosModeAll(device_id, mode, SERVOS2_GPIO_NUM_MAX, &operation_status);

        // Validate write operation / 验证写入操作
        if (chain_status != CHAIN_OK || operation_status != 1) {
            Serial.printf("Device ID[%d] set servo mode failed / 设备ID[%d]设置舵机模式失败\r\n", device_id, device_id);
            all_success = false;
            continue;
        }

        // Read verification: Confirm setting success / 读取验证：确认设置成功
        user_gpio_mode_t read_mode[SERVOS2_GPIO_NUM_MAX];
        if (M5Chain.getServosModeAll(device_id, read_mode, SERVOS2_GPIO_NUM_MAX) != CHAIN_OK) {
            Serial.printf("Device ID[%d] read servo mode failed / 设备ID[%d]读取舵机模式失败\r\n", device_id,
                          device_id);
            all_success = false;
            continue;
        }

        // Verify read data / 验证读取的数据
        bool mode_match = true;
        for (uint8_t j = 0; j < SERVOS2_GPIO_NUM_MAX; j++) {
            if (read_mode[j] != USER_GPIO_SERVO_MODE) {
                mode_match = false;
                break;
            }
        }

        if (mode_match) {
            Serial.printf(
                "Device ID[%d] servo mode set and verified successfully / 设备ID[%d]舵机模式设置并验证成功\r\n",
                device_id, device_id);
        } else {
            Serial.printf("Device ID[%d] servo mode verification failed / 设备ID[%d]舵机模式验证失败\r\n", device_id,
                          device_id);
            all_success = false;
        }
    }

    return all_success;
}

/**
 * Validate and set servo angles / 验证并设置舵机角度
 * @return Success status / 是否成功
 */
bool validateAndSetServoAngles()
{
    bool all_success = true;

    // Iterate through all devices / 遍历所有设备
    for (uint8_t i = 0; i < devices_list->count; i++) {
        // Check device type / 检查设备类型
        if (devices_list->devices[i].device_type != UNIT_8SERVOS2_CHAIN_TYPE_CODE) {
            continue;
        }

        uint8_t device_id = devices_list->devices[i].id;

        // Step 1: Read and verify current mode / 步骤1：读取并验证当前模式
        user_gpio_mode_t mode[SERVOS2_GPIO_NUM_MAX];
        if (M5Chain.getServosModeAll(device_id, mode, SERVOS2_GPIO_NUM_MAX) != CHAIN_OK) {
            Serial.printf("Device ID[%d] get servo mode failed / 设备ID[%d]获取舵机模式失败\r\n", device_id, device_id);
            all_success = false;
            continue;
        }

        // Verify all channel modes / 验证所有通道模式
        uint8_t err_count = 0;
        for (uint8_t j = 0; j < SERVOS2_GPIO_NUM_MAX; j++) {
            if (mode[j] != USER_GPIO_SERVO_MODE) {
                err_count++;
            }
        }

        if (err_count != 0) {
            Serial.printf("Device ID[%d] has %d channels with incorrect mode / 设备ID[%d]有%d个通道模式不正确\r\n",
                          device_id, err_count, device_id, err_count);
            all_success = false;
            continue;
        }

        // Step 2: Calculate and prepare new angles / 步骤2：计算并准备新角度
        angle += ANGLE_INCREMENT;
        if (angle > MAX_ANGLE) {
            angle = 0;
        }

        uint8_t angle_test[SERVOS2_GPIO_NUM_MAX];
        for (uint8_t j = 0; j < SERVOS2_GPIO_NUM_MAX; j++) {
            angle_test[j] = angle;
        }

        // Step 3: Write angle values / 步骤3：写入角度值
        chain_status = M5Chain.setServosAngleAll(device_id, angle_test, SERVOS2_GPIO_NUM_MAX, &operation_status);
        if (chain_status != CHAIN_OK) {
            Serial.printf(
                "Device ID[%d] set servo angle failed (communication error) / 设备ID[%d]设置舵机角度失败(通信错误)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        // Verify write operation status / 验证写入操作状态
        if (operation_status != 1) {
            Serial.printf(
                "Device ID[%d] set servo angle failed (operation status error) / "
                "设备ID[%d]设置舵机角度失败(操作状态异常)\r\n",
                device_id, device_id);
            all_success = false;
            continue;
        }

        // Step 4: Read and verify angle values / 步骤4：读取验证角度值
        uint8_t angle_get[SERVOS2_GPIO_NUM_MAX] = {0};
        if (M5Chain.getServosAngleAll(device_id, angle_get, SERVOS2_GPIO_NUM_MAX) != CHAIN_OK) {
            Serial.printf("Device ID[%d] get servo angle failed / 设备ID[%d]读取舵机角度失败\r\n", device_id,
                          device_id);
            all_success = false;
            continue;
        }

        // Step 5: Verify angle values per channel / 步骤5：逐通道验证角度值
        bool angle_match = true;
        for (uint8_t j = 0; j < SERVOS2_GPIO_NUM_MAX; j++) {
            if (angle_get[j] != angle_test[j]) {
                Serial.printf(
                    "Device ID[%d] channel %d angle mismatch: expected %d, actual %d / "
                    "设备ID[%d]通道%d角度不匹配：期望%d，实际%d\r\n",
                    device_id, j, angle_test[j], angle_get[j], device_id, j, angle_test[j], angle_get[j]);
                angle_match = false;
                all_success = false;
            }
        }

        if (angle_match) {
            Serial.printf(
                "Device ID[%d] angle set and verified successfully: %d° / 设备ID[%d]角度设置并验证成功：%d°\r\n",
                device_id, angle, device_id, angle);
        }
    }

    return all_success;
}

/**
 * Print device list information / 打印设备列表信息
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
 * Update device list / 更新设备列表
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
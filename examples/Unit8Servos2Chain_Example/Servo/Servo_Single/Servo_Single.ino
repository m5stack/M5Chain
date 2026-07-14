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
#define LOOP_DELAY_MS   (50)   // Loop delay in milliseconds / 循环延迟(毫秒)

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
 * Set each channel individually and verify the operation / 逐个通道设置并验证操作
 * @return Success status / 是否成功
 */
bool initializeServoModes()
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

        // Set servo mode for each channel individually / 逐个通道设置舵机模式
        for (uint8_t j = 0; j < SERVOS2_GPIO_NUM_MAX; j++) {
            // Write operation: Set servo mode / 写入操作：设置舵机模式
            chain_status = M5Chain.setServosMode(device_id, j, USER_GPIO_SERVO_MODE, &operation_status);

            // Validate write operation / 验证写入操作
            if (chain_status != CHAIN_OK) {
                Serial.printf(
                    "Device ID[%d] channel %d set servo mode failed (communication error) / "
                    "设备ID[%d]通道%d设置舵机模式失败(通信错误)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                continue;
            }

            if (operation_status != 1) {
                Serial.printf(
                    "Device ID[%d] channel %d set servo mode failed (operation status error) / "
                    "设备ID[%d]通道%d设置舵机模式失败(操作状态异常)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                continue;
            }

            // Read verification: Confirm mode was set correctly / 读取验证：确认模式设置正确
            user_gpio_mode_t read_mode;
            uint8_t read_status;

            if (M5Chain.getServosMode(device_id, j, &read_mode, &read_status) != CHAIN_OK) {
                Serial.printf("Device ID[%d] channel %d read servo mode failed / 设备ID[%d]通道%d读取舵机模式失败\r\n",
                              device_id, j, device_id, j);
                all_success = false;
                continue;
            }

            if (read_status != 1) {
                Serial.printf(
                    "Device ID[%d] channel %d read servo mode status error / 设备ID[%d]通道%d读取舵机模式状态错误\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                continue;
            }

            // Verify data consistency / 验证数据一致性
            if (read_mode != USER_GPIO_SERVO_MODE) {
                Serial.printf("Device ID[%d] channel %d mode verification failed / 设备ID[%d]通道%d模式验证失败\r\n",
                              device_id, j, device_id, j);
                all_success = false;
                continue;
            }

            Serial.printf(
                "Device ID[%d] channel %d servo mode set and verified successfully / "
                "设备ID[%d]通道%d舵机模式设置并验证成功\r\n",
                device_id, j, device_id, j);
        }
    }

    return all_success;
}

/**
 * Validate and set servo angles for all channels / 验证并设置所有通道的舵机角度
 * Process: Read mode -> Verify mode -> Write angle -> Read angle -> Verify angle /
 * 流程：读取模式 -> 验证模式 -> 写入角度 -> 读取角度 -> 验证角度
 * @return Success status / 是否成功
 */
bool validateAndSetServoAngles()
{
    // Calculate new angle / 计算新角度
    angle += ANGLE_INCREMENT;
    if (angle > MAX_ANGLE) {
        angle = 0;
    }

    bool all_success = true;

    // Iterate through all devices / 遍历所有设备
    for (uint8_t i = 0; i < devices_list->count; i++) {
        // Check device type / 检查设备类型
        if (devices_list->devices[i].device_type != UNIT_8SERVOS2_CHAIN_TYPE_CODE) {
            continue;
        }

        uint8_t device_id = devices_list->devices[i].id;

        // Process each channel / 处理每个通道
        for (uint8_t j = 0; j < SERVOS2_GPIO_NUM_MAX; j++) {
            user_gpio_mode_t mode;
            uint8_t angle_read;

            // Step 1: Read and verify current mode / 步骤1：读取并验证当前模式
            if (M5Chain.getServosMode(device_id, j, &mode, &operation_status) != CHAIN_OK) {
                Serial.printf(
                    "Device ID[%d] channel %d get servo mode failed (communication error) / "
                    "设备ID[%d]通道%d获取舵机模式失败(通信错误)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                break;
            }

            // Validate read operation status / 验证读取操作状态
            if (operation_status != 1) {
                Serial.printf(
                    "Device ID[%d] channel %d get servo mode failed (operation status error) / "
                    "设备ID[%d]通道%d获取舵机模式失败(操作状态异常)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                break;
            }

            // Verify mode is correct / 验证模式正确性
            if (mode != USER_GPIO_SERVO_MODE) {
                Serial.printf(
                    "Device ID[%d] channel %d mode mismatch: expected SERVO_MODE, got %d / "
                    "设备ID[%d]通道%d模式不匹配：期望SERVO_MODE，实际%d\r\n",
                    device_id, j, mode, device_id, j, mode);
                all_success = false;
                break;
            }

            // Step 2: Write angle value / 步骤2：写入角度值
            if (M5Chain.setServosAngle(device_id, j, angle, &operation_status) != CHAIN_OK) {
                Serial.printf(
                    "Device ID[%d] channel %d set servo angle failed (communication error) / "
                    "设备ID[%d]通道%d设置舵机角度失败(通信错误)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                break;
            }

            // Validate write operation status / 验证写入操作状态
            if (operation_status != 1) {
                Serial.printf(
                    "Device ID[%d] channel %d set servo angle failed (operation status error) / "
                    "设备ID[%d]通道%d设置舵机角度失败(操作状态异常)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                break;
            }

            // Step 3: Read back angle value / 步骤3：读取角度值
            if (M5Chain.getServosAngle(device_id, j, &angle_read, &operation_status) != CHAIN_OK) {
                Serial.printf(
                    "Device ID[%d] channel %d get servo angle failed (communication error) / "
                    "设备ID[%d]通道%d读取舵机角度失败(通信错误)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                break;
            }

            // Validate read operation status / 验证读取操作状态
            if (operation_status != 1) {
                Serial.printf(
                    "Device ID[%d] channel %d get servo angle failed (operation status error) / "
                    "设备ID[%d]通道%d读取舵机角度失败(操作状态异常)\r\n",
                    device_id, j, device_id, j);
                all_success = false;
                break;
            }

            // Step 4: Verify angle consistency / 步骤4：验证角度一致性
            if (angle_read != angle) {
                Serial.printf(
                    "Device ID[%d] channel %d angle mismatch: expected %d°, actual %d° / "
                    "设备ID[%d]通道%d角度不匹配：期望%d°，实际%d°\r\n",
                    device_id, j, angle, angle_read, device_id, j, angle, angle_read);
                all_success = false;
                break;
            }
        }
    }

    if (all_success) {
        Serial.printf("All channels angle set and verified successfully: %d° / 所有通道角度设置并验证成功：%d°\r\n",
                      angle, angle);
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
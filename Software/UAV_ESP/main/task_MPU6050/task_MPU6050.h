#ifndef TASK_MPU6050_H
#define TASK_MPU6050_H

#include <driver/i2c.h>
#include <esp_log.h>
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "../../Hardware/MPU6050/MPU6050.h"
#include "sdkconfig.h"

// 函数声明
void task_MPU6050(void*);

// 全局变量定义
extern uint8_t receive_buffer[];
extern Quaternion q;           // [w, x, y, z]         四元数容器
extern VectorFloat gravity;    // [x, y, z]            重力向量
extern float ypr[];           // [yaw, pitch, roll]   偏航/俯仰/翻滚容器和重力向量
extern uint16_t packetSize;    // 预期的DMP数据包大小（默认为42字节）
extern uint16_t fifoCount;     // FIFO中当前所有字节的计数
extern uint8_t fifoBuffer[]; // FIFO存储缓冲区
extern uint8_t mpuIntStatus;   // 保存来自MPU的实际中断状态字节
#endif
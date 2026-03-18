#ifndef TASK_ATTITUDE_H
#define TASK_ATTITUDE_H

#include <driver/i2c.h>
#include <esp_log.h>
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "../../Hardware/MPU6050/MPU6050.h"
#include "../../Hardware/QMC5883L/QMC5883L.h"
#include "../../Hardware/BMP280/BMP280.h"
#include "sdkconfig.h"

// 任务入口
void task_attitude(void*);

// ===== MPU6050 姿态数据 =====
extern uint8_t receive_buffer[];
extern Quaternion q;           // [w, x, y, z]         四元数
extern VectorFloat gravity;    // [x, y, z]            重力向量
extern float ypr[];            // [yaw, pitch, roll]   偏航/俯仰/翻滚
extern uint16_t packetSize;    // DMP数据包大小
extern uint16_t fifoCount;     // FIFO字节计数
extern uint8_t fifoBuffer[];   // FIFO缓冲区
extern uint8_t mpuIntStatus;   // MPU中断状态字节

// ===== QMC5883L 磁力计数据 =====
extern float mag_azimuth;      // 方位角 (0-360度)
extern float mag_x;            // X轴磁场（校准后）
extern float mag_y;            // Y轴磁场（校准后）
extern float mag_z;            // Z轴磁场（校准后）

// ===== BMP280 气压计数据 =====
extern float baro_temperature; // 温度 (℃)
extern float baro_pressure;    // 气压 (Pa)
extern float baro_altitude;    // 海拔高度 (m)

#endif // TASK_ATTITUDE_H

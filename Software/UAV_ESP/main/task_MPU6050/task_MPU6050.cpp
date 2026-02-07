#include "task_MPU6050.h"
#include "../../Hardware/MPU6050/MPU6050_6Axis_MotionApps20.h"

#define PIN_SDA 8
#define PIN_CLK 9

static const char *TAG = "MPU6050";

// 全局变量定义
uint8_t receive_buffer[255];
Quaternion q;           // [w, x, y, z]         四元数容器
VectorFloat gravity;    // [x, y, z]            重力向量
float ypr[3];           // [yaw, pitch, roll]   偏航/俯仰/翻滚容器和重力向量
uint16_t packetSize = 42;    // 预期的DMP数据包大小（默认为42字节）
uint16_t fifoCount;     // FIFO中当前所有字节的计数
uint8_t fifoBuffer[64]; // FIFO存储缓冲区
uint8_t mpuIntStatus;   // 保存来自MPU的实际中断状态字节


void task_MPU6050(void*){

    I2Cdev::initI2C(PIN_SDA, PIN_CLK);
    MPU6050 mpu = MPU6050();
    mpu.initialize();
    mpu.dmpInitialize();
    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);
  
      mpu.setDMPEnabled(true);
  
      while(1){
        // 获取中断状态
        mpuIntStatus = mpu.getIntStatus();
        // 获取FIFO计数
        fifoCount = mpu.getFIFOCount();
  
          if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
              // 重置FIFO
              mpu.resetFIFO();
  
          } else if (mpuIntStatus & 0x02) {
              // 等待FIFO计数小于packetSize
              while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
  
            // 获取FIFO数据
            mpu.getFIFOBytes(fifoBuffer, packetSize);
            // 获取四元数
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            // 获取重力向量
            mpu.dmpGetGravity(&gravity, &q);
            // 获取偏航/俯仰/翻滚
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

          }
      }
      // 删除任务
      vTaskDelete(NULL);
  }
  
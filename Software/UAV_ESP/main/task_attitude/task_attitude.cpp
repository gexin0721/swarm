#include "task_attitude.h"
#include "../../Hardware/MPU6050/MPU6050_6Axis_MotionApps20.h"

#define PIN_SDA 8
#define PIN_CLK 9

static const char *TAG = "ATTITUDE";

// ===== MPU6050 全局变量 =====
uint8_t receive_buffer[255];
Quaternion q;           // [w, x, y, z]         四元数
VectorFloat gravity;    // [x, y, z]            重力向量
float ypr[3];           // [yaw, pitch, roll]   偏航/俯仰/翻滚
uint16_t packetSize = 42;
uint16_t fifoCount;
uint8_t fifoBuffer[64];
uint8_t mpuIntStatus;

// ===== QMC5883L 全局变量 =====
float mag_azimuth = 0.0f;
float mag_x = 0.0f;
float mag_y = 0.0f;
float mag_z = 0.0f;

// ===== BMP280 全局变量 =====
float baro_temperature = 0.0f;
float baro_pressure    = 0.0f;
float baro_altitude    = 0.0f;

// ===== I2C 回调函数（供 QMC5883L / BMP280 使用）=====
static bool i2c_write_cb(uint8_t dev_addr, uint8_t reg_addr, uint8_t data) {
    return I2Cdev::writeByte(dev_addr, reg_addr, data);
}

static bool i2c_read_cb(uint8_t dev_addr, uint8_t reg_addr, uint8_t *buf, uint8_t length) {
    return I2Cdev::readBytes(dev_addr, reg_addr, length, buf) == length;
}

void task_attitude(void*) {

    // ===== I2C 总线初始化 =====
    I2Cdev::initI2C((gpio_num_t)PIN_SDA, (gpio_num_t)PIN_CLK);

    // // ===== MPU6050 初始化 =====
    // MPU6050 mpu = MPU6050();
    // mpu.initialize();
    // mpu.dmpInitialize();
    // mpu.CalibrateAccel(6);
    // mpu.CalibrateGyro(6);
    // mpu.setDMPEnabled(true);
    // ESP_LOGI(TAG, "MPU6050 初始化完成");

    // ===== QMC5883L 初始化 =====
    QMC5883L qmc(i2c_write_cb, i2c_read_cb);
    if (qmc.init()) {
        ESP_LOGI(TAG, "QMC5883L 初始化完成");
    } else {
        ESP_LOGW(TAG, "QMC5883L 初始化失败");
    }

    // ===== BMP280 初始化 =====
    BMP280 bmp(i2c_write_cb, i2c_read_cb);
    if (bmp.init()) {
        ESP_LOGI(TAG, "BMP280 初始化完成");
    } else {
        ESP_LOGW(TAG, "BMP280 初始化失败");
    }

    while (1) {
        // ===== MPU6050 读取 =====
        // mpuIntStatus = mpu.getIntStatus();
        // fifoCount    = mpu.getFIFOCount();

        // if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        //     mpu.resetFIFO();
        // } else if (mpuIntStatus & 0x02) {
        //     while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
        //     mpu.getFIFOBytes(fifoBuffer, packetSize);
        //     mpu.dmpGetQuaternion(&q, fifoBuffer);
        //     mpu.dmpGetGravity(&gravity, &q);
        //     mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        // }

        // ===== QMC5883L 读取 =====
        if (qmc.read()) {
            mag_azimuth = qmc.getAzimuth();
            qmc.getCalibratedData(mag_x, mag_y, mag_z);
        }

        // ===== BMP280 读取 =====
        bmp.getMeasurements(baro_temperature, baro_pressure, baro_altitude);

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

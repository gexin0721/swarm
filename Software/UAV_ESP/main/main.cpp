#include "main.h"
#include "task_attitude/task_attitude.h"

static const char *TAG = "MAIN";

extern "C" int app_main(void)
{

    // ============= 硬件初始化 =============
    // 多线程处理
    // 姿态管理线程（MPU6050 + QMC5883L + BMP280）
    xTaskCreate(&task_attitude, "attitude_task", 8192, NULL, 5, NULL);


    while(1){

        // 打印偏航/俯仰/翻滚
        ESP_LOGI(TAG, "YAW: %3.1f, PITCH: %3.1f, ROLL: %3.1f", 
                 ypr[0] * 180/M_PI, ypr[1] * 180/M_PI, ypr[2] * 180/M_PI);

        // 延时1秒
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    return 0;
}


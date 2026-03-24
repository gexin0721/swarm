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

        // 打印气压计数据
        ESP_LOGI(TAG, "TEMP: %.2f C, PRES: %.2f Pa, ALT: %.2f m",
                 baro_temperature, baro_pressure, baro_altitude);

        // 延时1秒
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    return 0;
}


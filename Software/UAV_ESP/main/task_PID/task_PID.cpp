#include "task_PID.h"

const char* PID_key[4][3] = {{"pid_kp0", "pid_ki0", "pid_kd0"}, 
                            {"pid_kp1", "pid_ki1", "pid_kd1"}, 
                            {"pid_kp2", "pid_ki2", "pid_kd2"}, 
                            {"pid_kp3", "pid_ki3", "pid_kd3"}};

#define CONTROL_PERIOD_MS 10  // 100Hz控制频率

void init_nvs(){
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        err = nvs_flash_init();
    }

    // 打开NVS命名空间
    nvs_open("storage", NVS_READWRITE, &PID_handle);
    
    // 检查是否有参数，如果没有则初始化为0
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 3; j++){
            float value;
            esp_err_t ret = nvs_get_float(PID_handle, PID_key[i][j], &value);
            
            // 如果读取失败（不存在），则设置为0
            if(ret == ESP_ERR_NVS_NOT_FOUND){
                nvs_set_float(PID_handle, PID_key[i][j], 0.0);
            }
        }
    }
    
    // 提交更改
    nvs_commit(PID_handle);
    nvs_close(PID_handle);
}



void task_PID(void*){

    init_nvs();
    // 硬件初始化
    LED led(GPIO_NUM_10);
    Buzzer buzzer(GPIO_NUM_11);

    // PWM初始化
    PWM pwm(GPIO_NUM_42, GPIO_NUM_41, GPIO_NUM_38, GPIO_NUM_35);
    // 电机控制
    Control control[4] = {
        Control(GPIO_NUM_1, GPIO_NUM_2, LEDC_LOW_SPEED_MODE, GPIO_NUM_42),
        Control(GPIO_NUM_3, GPIO_NUM_4, LEDC_LOW_SPEED_MODE, GPIO_NUM_41),
        Control(GPIO_NUM_40, GPIO_NUM_39, LEDC_LOW_SPEED_MODE, GPIO_NUM_38),
        Control(GPIO_NUM_37, GPIO_NUM_36, LEDC_LOW_SPEED_MODE, GPIO_NUM_35)
    };

    // 读取PID参数
    nvs_open("storage", NVS_READONLY, &PID_handle);
    float kp[4];
    float ki[4];
    float kd[4];

    for(int i = 0; i < 4; i++){
        nvs_get_float(PID_handle, PID_key[i][0], &kp[i]);
        nvs_get_float(PID_handle, PID_key[i][1], &ki[i]);
        nvs_get_float(PID_handle, PID_key[i][2], &kd[i]);
    }
    nvs_close(PID_handle);

    PID pid[4] = {
        PID(kp[0], ki[0], kd[0]),
        PID(kp[1], ki[1], kd[1]),
        PID(kp[2], ki[2], kd[2]),
        PID(kp[3], ki[3], kd[3])
    };



    while(1){

        
        vTaskDelay(CONTROL_PERIOD_MS/portTICK_PERIOD_MS);
    }
}
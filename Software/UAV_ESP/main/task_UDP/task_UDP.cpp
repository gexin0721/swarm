#include "task_UDP.h"
#include "./ROS_DATA_RX/ROS_DATA_RX.h"
#include "./ROS_DATA_TX/ROS_DATA_TX.h"

uint8_t DUP_id = 0;
static const char* TAG = "UDP_TASK";

/**
 * @brief 向上位机注册并获取无人机ID
 * @param udp UDP对象指针
 * @param local_ip 本机IP地址
 * @param local_port 本机端口
 * @return 成功返回分配的ID，失败返回0xFF
 */
uint8_t register_drone(UDP* udp)
{
    // 构造JSON格式: {"ip":"192.168.1.100","port":8888}
    char json_buffer[128];
    snprintf(json_buffer, sizeof(json_buffer), "{\"ip\":\"%s\",\"port\":%d}", udp->local_ip, udp->local_port);
        
    // 发送IP和端口给上位机
    udp->send_data((uint8_t*)json_buffer, strlen(json_buffer));
    
    // 等待接收上位机分配的ID (超时5秒)
    uint8_t buffer[256];
    uint32_t timeout = 5000;
    uint32_t start = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    while ((xTaskGetTickCount() * portTICK_PERIOD_MS - start) < timeout) {
        int len = udp->receive_data(buffer, sizeof(buffer));
        if (len > 0) {
            // 上位机返回分配的ID (单字节)
            return buffer[0];
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
    return 0xFF; // 注册失败
}


void udp_tasck(void *pvParameter)
{
    // 初始化UDP
    UDP udp;
    DUP_id = register_drone(&udp);
    if(DUP_id == 0xFF)
    {
        ESP_LOGE(TAG, "注册失败");
        vTaskDelete(NULL);  // 删除当前任务
        return;
    }

    // 初始化ROS_DATA_RX
    ROS_DATA_RX ros_data_rx(DUP_id);
    
    // 初始化ROS_DATA_TX（使用UDP实例）
    ROS_DATA_TX ros_data_tx(&udp, DUP_id);
    
    // 先获取 UDP 数据
    uint8_t buffer[256];
    while (1)
    {

        int len = udp.receive_data(buffer, sizeof(buffer));
        if(len > 0)
        {
            // 解析数据
            ros_data_rx.parse_data(data, len);
        }


    }
}




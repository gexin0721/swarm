#include "UDP.h"
#include "esp_netif.h"
#include "esp_wifi.h"

static const char *TAG = "UDP";

//  ================ 创建UDP客户端对象 ================
UDP::UDP(const char* server_ip, uint16_t server_port, uint16_t local_port)
{
    // 初始化连接状态
    is_connected = false;
    this->local_port = local_port;
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&local_addr, 0, sizeof(local_addr));
    memset(this->local_ip, 0, sizeof(this->local_ip));

    ESP_LOGI(TAG, "UDP客户端对象创建完成");
    
    // 获取ESP32的实际IP地址
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    if (netif != NULL) {
        esp_netif_ip_info_t ip_info;
        if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK) {
            // 将IP地址转换为字符串格式
            snprintf(this->local_ip, sizeof(this->local_ip), IPSTR, IP2STR(&ip_info.ip));
            ESP_LOGI(TAG, "获取到本地IP地址: %s", this->local_ip);
        } else {
            ESP_LOGW(TAG, "获取IP地址失败");
            strcpy(this->local_ip, "0.0.0.0");
        }
    } else {
        ESP_LOGW(TAG, "未找到WIFI网络接口");
        strcpy(this->local_ip, "0.0.0.0");
    }

    // 创建UDP套接字
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        ESP_LOGE(TAG, "创建套接字失败");
        exit(1);
    }
    
    // 配置本地地址并绑定端口
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;  // 绑定所有本地网络接口
    local_addr.sin_port = htons(local_port);
    
    // 绑定本地端口
    if (bind(socket_fd, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        ESP_LOGE(TAG, "绑定本地端口 %d 失败，错误码: %d", local_port, errno);
        close(socket_fd);
        socket_fd = -1;
        exit(1);
    }
    ESP_LOGI(TAG, "成功绑定本地端口: %d", local_port);
    
    // 配置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    
    // 将IP地址字符串转换为网络地址
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        ESP_LOGE(TAG, "IP地址配置失败: %s", server_ip);
        close(socket_fd);
        socket_fd = -1;
        exit(1);
    }
    
    // 设置套接字为非阻塞模式
    int flags = fcntl(socket_fd, F_GETFL, 0);
    fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
    
    is_connected = true;
    ESP_LOGI(TAG, "已连接到UDP服务器 %s:%d (本地 %s:%d)", server_ip, server_port, this->local_ip, local_port);

}

//  ================ 释放UDP资源 ================
UDP::~UDP()
{
    if (socket_fd >= 0) {
        ESP_LOGI(TAG, "关闭UDP连接");
        close(socket_fd);
        socket_fd = -1;
        is_connected = false;
    }
}

//  ================ 发送数据到服务器 ================
bool UDP::send_data(uint8_t* data, uint8_t length)
{
    if (!is_connected || socket_fd < 0) {
        ESP_LOGE(TAG, "UDP未连接到服务器");
        return false;
    }
    
    // 发送数据到服务器
    // 参数：1. 套接字文件描述符 2. 数据 3. 数据长度 4. 0 5. 服务器地址 6. 服务器地址长度
    ssize_t sent_bytes = sendto(socket_fd, data, length, 0,
                        (struct sockaddr*)&server_addr, sizeof(server_addr));

    if (sent_bytes == length) {
        ESP_LOGD(TAG, "成功发送数据到服务器，长度: %d", length);
        return true;
    } else {
        ESP_LOGE(TAG, "发送数据到服务器失败，发送字节: %d，期望字节: %d", (int)sent_bytes, length);
        return false;
    }
}
//  ================ 发送JSON数据到服务器 ================
bool UDP::send_data(const std::string& json_data)
{
    if (!is_connected || socket_fd < 0) {
        ESP_LOGE(TAG, "UDP未连接到服务器");
        return false;
    }
    
    // 发送JSON数据到服务器
    // 参数：1. 套接字文件描述符 2. JSON字符串数据 3. 数据长度 4. 0 5. 服务器地址 6. 服务器地址长度
    ssize_t sent_bytes = sendto(socket_fd, json_data.c_str(), json_data.length(), 0,
                        (struct sockaddr*)&server_addr, sizeof(server_addr));

    if (sent_bytes == json_data.length()) {
        ESP_LOGD(TAG, "成功发送JSON数据到服务器，长度: %d", (int)json_data.length());
        return true;
    } else {
        ESP_LOGE(TAG, "发送JSON数据到服务器失败，发送字节: %d，期望字节: %d", (int)sent_bytes, (int)json_data.length());
        return false;
    }
}
//  ================ 从服务器接收数据 ================
// 参数一 （）： 
int UDP::receive_data(uint8_t* buffer, uint8_t max_length)
{
    if (!is_connected || socket_fd < 0) {
        return -1;
    }
    
    // 从服务器接收数据
    socklen_t server_addr_len = sizeof(server_addr);
    ssize_t received_bytes = recvfrom(socket_fd, buffer, max_length, 0,
                                (struct sockaddr*)&server_addr, &server_addr_len);

    if (received_bytes < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;  // 无数据可读
        } else {
            ESP_LOGE(TAG, "从服务器接收数据失败，错误码: %d", errno);
            return -1;
        }
    }
    
    ESP_LOGD(TAG, "从服务器接收数据成功，长度: %d", (int)received_bytes);
    return (int)received_bytes;
}






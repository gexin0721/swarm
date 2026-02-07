#ifndef UDP_H
#define UDP_H

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

class UDP {
private:
    // 套接字文件描述符
    int socket_fd = -1;
    // 服务器地址结构
    struct sockaddr_in server_addr;
    // 本地地址结构
    struct sockaddr_in local_addr;
    // 连接状态标志
    bool is_connected = false;

public:
    // 本地端口（公有，供外部访问）
    uint16_t local_port = 0;
    // 本地IP地址（公有，供外部访问，如用于注册）
    char local_ip[16] = {0};
    // 构造函数
    // 参数1: 服务器IP地址
    // 参数2: 服务器端口
    // 参数3: 本地绑定端口（0表示自动分配）
    UDP(const char* server_ip = "192.168.0.102", uint16_t server_port = 8888, uint16_t local_port = 9999);
    
    // 析构函数
    ~UDP();
    
    // 发送数据到服务器
    bool send_data(uint8_t* data, uint8_t length);
    
    // 从服务器接收数据（非阻塞）
    int receive_data(uint8_t* buffer, uint8_t max_length);
    
    // 获取连接状态
    bool is_connected_status() const { return is_connected; }
    
    
};


#endif // UDP_H

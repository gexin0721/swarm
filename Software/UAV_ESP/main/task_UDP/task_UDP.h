#ifndef TASK_UDP_H
#define TASK_UDP_H
// 转化工具
#include <iostream> // 标准输入输出流
#include <string> // 字符串
#include <vector> // 向量容器
#include <queue> // 队列容器
#include <mutex> // 互斥锁
#include <thread> // 线程
#include <chrono> // 时间
#include <json/json.h> // JSON库
#include <sstream> // 字符串流
#include <algorithm> // 算法
#include "../../Hardware/UDP/UDP.h" // UDP类
#include "semaphore.h" // 信号量
#include "event_groups.h" // 事件组
#include "freertos/FreeRTOS.h" // FreeRTOS库
#include "freertos/task.h" // 任务
#include "freertos/queue.h" // 队列
#include "freertos/semaphore.h" // 信号量
#include "freertos/event_groups.h" // 事件组
#include <nvs_flash.h> // NVS闪存库
#include <nvs.h> // NVS库

#include "../../System/sys/sys.h" 

// UDP线程（主要是，解析各类数据，并放入队列）
void udp_task(void *pvParameter);
#endif
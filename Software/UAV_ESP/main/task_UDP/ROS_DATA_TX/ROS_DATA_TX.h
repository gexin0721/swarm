#ifndef ROS_DATA_TX_H
#define ROS_DATA_TX_H

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <cstdint>
#include "../../Hardware/UDP/UDP.h"

/**
 * @brief 无人机数据发送类
 * @details 该类负责将无人机的各类数据打包成二进制格式并发送
 *          支持的数据类型：姿态、GPS、电池、ID、PID参数等
 */
class ROS_DATA_TX
{
public:
    // 发送函数指针类型定义
    typedef bool (*send_u8_array_t)(uint8_t* data, uint8_t length);

    /**
     * @brief PID参数结构体
     */
    struct PID
    {
        float kp;  // 比例系数
        float ki;  // 积分系数
        float kd;  // 微分系数
    };

private:
    send_u8_array_t send_u8_array;  // 发送函数指针
    UDP* udp_instance;               // UDP实例指针
    uint8_t id;                      // 无人机ID

    /**
     * @brief 计算数据包校验和
     * @param length 数据包长度（不包括包尾）
     * @param data 数据包指针
     * @return 校验和
     */
    uint8_t parse_checksum(uint8_t length, uint8_t* data);

public:
    /**
     * @brief 构造函数（使用函数指针）
     * @param send_u8_array_func 发送函数指针
     * @param drone_id 无人机ID，默认为0
     */
    ROS_DATA_TX(send_u8_array_t send_u8_array_func, uint8_t drone_id = 0)
        : send_u8_array(send_u8_array_func), udp_instance(nullptr), id(drone_id)
    {
    }

    /**
     * @brief 构造函数（使用UDP实例）
     * @param udp UDP实例指针
     * @param drone_id 无人机ID，默认为0
     */
    ROS_DATA_TX(UDP* udp, uint8_t drone_id = 0);

    /**
     * @brief 发送姿态数据
     * @param roll 横滚角
     * @param pitch 俯仰角
     * @param yaw 偏航角
     * @return 发送成功返回true
     */
    bool posture(float roll, float pitch, float yaw);

    /**
     * @brief 发送GPS位置数据
     * @param x X坐标
     * @param y Y坐标
     * @param z Z坐标
     * @return 发送成功返回true
     */
    bool gps(float x, float y, float z);

    /**
     * @brief 发送电池电压数据
     * @param batt 电池电压值
     * @return 发送成功返回true
     */
    bool battery(uint8_t batt);

    /**
     * @brief 发送无人机ID
     * @param drone_id 无人机ID
     * @return 发送成功返回true
     */
    bool send_id(uint8_t drone_id);

    /**
     * @brief 发送一号电机PID参数
     * @param kp 比例系数（float）
     * @param ki 积分系数（float）
     * @param kd 微分系数（float）
     * @return 发送成功返回true
     */
    bool pid_motor_1(float kp, float ki, float kd);

    /**
     * @brief 发送二号电机PID参数
     * @param kp 比例系数（float）
     * @param ki 积分系数（float）
     * @param kd 微分系数（float）
     * @return 发送成功返回true
     */
    bool pid_motor_2(float kp, float ki, float kd);

    /**
     * @brief 发送三号电机PID参数
     * @param kp 比例系数（float）
     * @param ki 积分系数（float）
     * @param kd 微分系数（float）
     * @return 发送成功返回true
     */
    bool pid_motor_3(float kp, float ki, float kd);

    /**
     * @brief 发送四号电机PID参数
     * @param kp 比例系数（float）
     * @param ki 积分系数（float）
     * @param kd 微分系数（float）
     * @return 发送成功返回true
     */
    bool pid_motor_4(float kp, float ki, float kd);

    /**
     * @brief 发送所有数据（一次性发送所有信息）
     * @param drone_id 无人机ID
     * @param roll_val 横滚角
     * @param pitch_val 俯仰角
     * @param yaw_val 偏航角
     * @param x_val X坐标
     * @param y_val Y坐标
     * @param z_val Z坐标
     * @param batt_val 电池电压
     * @param pid0_kp~pid3_kd 四个电机的PID参数（float）
     * @return 发送成功返回true
     */
    bool all(uint8_t drone_id, float roll_val, float pitch_val, float yaw_val, 
             float x_val, float y_val, float z_val, uint8_t batt_val,
             float pid0_kp, float pid0_ki, float pid0_kd,
             float pid1_kp, float pid1_ki, float pid1_kd,
             float pid2_kp, float pid2_ki, float pid2_kd,
             float pid3_kp, float pid3_ki, float pid3_kd);

    // Getter和Setter方法
    void set_id(uint8_t drone_id) { id = drone_id; }
    uint8_t get_id() const { return id; }
};

#endif
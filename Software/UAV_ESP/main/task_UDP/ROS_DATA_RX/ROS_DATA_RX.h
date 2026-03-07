#ifndef ROS_DATA_RX_H
#define ROS_DATA_RX_H

#include <vector>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <stdexcept>
using namespace std;

// ==================== 数据类型代表位 ====================
// 目标方向 0x00
#define DATA_TYPE_TARGET_DIRECTION 0x00
// 目标位置 0x01
#define DATA_TYPE_TARGET_POSITION 0x01
// 目标速度 0x02
#define DATA_TYPE_TARGET_VELOCITY 0x02
// 修改PID参数 电机一号 0x03
#define DATA_TYPE_TARGET_PID_MOTOR_1 0x03
// 修改PID参数 电机二号 0x04
#define DATA_TYPE_TARGET_PID_MOTOR_2 0x04
// 修改PID参数 电机三号 0x05
#define DATA_TYPE_TARGET_PID_MOTOR_3 0x05
// 修改PID参数 电机四号 0x06
#define DATA_TYPE_TARGET_PID_MOTOR_4 0x06
// 走 X 个 单位距离 ， 旋转 Y 个单位角度 0x07
#define DATA_TYPE_TARGET_MOVE_DISTANCE_AND_ROTATE_ANGLE 0x07

//  ==================== 二进制数据 ====================
struct BinaryData
{
    uint8_t* data;
    uint8_t length;
};
struct _Data
{
    _Data(uint8_t type) : type(type){}

    uint8_t type;
    vector<float> data;
};

// ==================== 解析数据 ====================
class ROS_DATA_RX
{

private:
    // 数据队列
    vector<_Data> data_queue;
    // 无人机ID
    uint8_t id = 0;
public:

    // 解析校验位
    // 校验位 = ( 状态位 + 数据长度 + 参数位1 + 参数位2 + ... + 参数位n) & 0xFF

    uint8_t parse_checksum(uint8_t length, uint8_t* data);
    // 解析数据,然后存入数据队列
    // [包头0][包头1][ID位][状态位][数据长度][参数位1][参数位2]...[参数位n][校验位][包尾]
    bool parse_data(BinaryData data);

    // 构造函数
    ROS_DATA_RX(uint8_t id)
    {
        this->id = id;
    }
    // 析构函数
    ~ROS_DATA_RX()
    {
        data_queue.clear();
    }

    // 尾部删除元素
    void pop_back() 
    {
        data_queue.pop_back();
    }

    // 清空数据
    void clear()
    {
        data_queue.clear();
    }

    // 获取数据长度
    uint8_t size()
    {
        return data_queue.size();
    }

    // 判空
    bool empty()
    {
        return data_queue.empty();
    }
    
    // 获取数据队列
    vector<_Data>& get_data_queue() 
    {
        return data_queue;
    }

    // 队列出队函数 - 从头部删除并返回第一个元素
    _Data pop_front()
    {
        if(data_queue.empty())
        {
            throw std::runtime_error("ROS_DATA_RX队列为空，无法出队");
        }
        _Data front_data = data_queue.front();
        data_queue.erase(data_queue.begin());
        return front_data;
    }

    // 运算符重载
    _Data& operator[](int index)//获取数据
    {
        if(index < 0 || index >= data_queue.size())
        {
            throw std::out_of_range("ROS_DATA_RX下标越界");
        }
        return data_queue[index];
    }

    // 迭代器
    vector<_Data>::iterator begin() 
    {
        return data_queue.begin();
    }
    vector<_Data>::iterator end() 
    {
        return data_queue.end();
    }

    // 迭代器
    vector<_Data>::const_iterator begin() const
    {
        return data_queue.begin();
    }
    vector<_Data>::const_iterator end() const
    {
        return data_queue.end();
    }
};



#endif
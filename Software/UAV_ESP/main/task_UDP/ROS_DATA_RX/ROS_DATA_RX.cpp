// ROS_DATA.cpp
// 解析数据
#include "ROS_DATA_RX.h"


uint8_t ROS_DATA_RX::parse_checksum(uint8_t length, uint8_t* data)
{
    int checksum = 0;
    checksum += data[3]; // 状态位
    checksum += data[4]; // 数据长度
    for(int i = 5; i < length - 2; i++)
    {
        checksum += data[i];
    }
    return (uint8_t)(checksum & 0xFF);
}

const char* PID_key[4][3] = {{"pid_kp0", "pid_ki0", "pid_kd0"}, 
                            {"pid_kp1", "pid_ki1", "pid_kd1"}, 
                            {"pid_kp2", "pid_ki2", "pid_kd2"}, 
                            {"pid_kp3", "pid_ki3", "pid_kd3"}};

// ==================== 解析数据 ====================
// 函数名 ： 解析数据,然后存入数据队列
// 参数 ： 二进制数据
// 返回值 ： 是否解析成功
bool ROS_DATA_RX::parse_data(BinaryData data)
{
    // 最小数据包长度: 包头(2) + ID(1) + 状态位(1) + 数据长度(1) + 校验位(1) + 包尾(1) = 7
    if(data.length < 7 || data.data == nullptr || data.data[0] != 0xEE || data.data[1] != 0xEE)
    {
        std::cout << "数据不合法" << std::endl;
        return false;
    }
    if(data.data[2] != id)
    {
        std::cout << "数据与无人机无关，当前ID：" << id << "，接收ID：" << data.data[2] << std::endl;
        return false;
    }

    // 解析校验位
    uint8_t checksum = parse_checksum(data.length, data.data);

    // 后续不需要再校验长度了，因为如果长度不够，这里过不去
    if(checksum != data.data[data.length - 2]) // 校验位在倒数第二位
    {
        std::cout << "校验位错误" << std::endl;
        return false;
    }
    // 检查包尾
    if(data.data[data.length - 1] != 0xFF) // 包尾在最后一位
    {
        std::cout << "包尾错误" << std::endl;
        return false;
    }

    _Data data_struct(data.data[3]);
    
    // 解析数据
    // 后续不需要再校验长度，因为如果长度不够，校验和根本过不去，不必过多考虑数据长度
    switch(data.data[3])
    {

        case 0x00: // 目标方向 翻滚角 俯仰角 偏航角
        case 0x01: // 目标位置
        case 0x02: // 目标速度
        {
            // 将4个字节组合成32位整数，然后转换为float
            int32_t temp0 = (data.data[5]  << 24 | data.data[6]  << 16 | data.data[7]  << 8 | data.data[8]);
            int32_t temp1 = (data.data[9]  << 24 | data.data[10] << 16 | data.data[11] << 8 | data.data[12]);
            int32_t temp2 = (data.data[13] << 24 | data.data[14] << 16 | data.data[15] << 8 | data.data[16]);
            data_struct.data.push_back(*(float*)&temp0);
            data_struct.data.push_back(*(float*)&temp1);
            data_struct.data.push_back(*(float*)&temp2);
            break;
        }
        case 0x03: //  修改PID参数 电机一号
        case 0x04: //  修改PID参数 电机二号
        case 0x05: //  修改PID参数 电机三号
        case 0x06: //  修改PID参数 电机四号
        {
            // 将4个字节组合成32位整数，然后转换为float
            int32_t temp0 = (data.data[5]  << 24 | data.data[6]  << 16 | data.data[7]  << 8 | data.data[8]);
            int32_t temp1 = (data.data[9]  << 24 | data.data[10] << 16 | data.data[11] << 8 | data.data[12]);
            int32_t temp2 = (data.data[13] << 24 | data.data[14] << 16 | data.data[15] << 8 | data.data[16]);
            data_struct.data.push_back(*(float*)&temp0);
            data_struct.data.push_back(*(float*)&temp1);
            data_struct.data.push_back(*(float*)&temp2);

            // 更新闪存数据
            nvs_open("storage", NVS_READWRITE, &PID_handle);
            nvs_set_float(PID_handle, PID_key[data.data[4] - 0x03][0], *(float*)&temp0);
            nvs_set_float(PID_handle, PID_key[data.data[4] - 0x03][1], *(float*)&temp1);
            nvs_set_float(PID_handle, PID_key[data.data[4] - 0x03][2], *(float*)&temp2);
            nvs_commit(PID_handle);
            nvs_close(PID_handle);
            break;
        }
        case 0x07: //  走 X 个 单位距离 ， 旋转 Y 个单位角度
        {

            // 将2个字节组合成16位有符号整数，支持负数
            int16_t distance = (int16_t)((data.data[5] << 8) | data.data[6]);
            int16_t angle = (int16_t)((data.data[7] << 8) | data.data[8]);
            data_struct.data.push_back((float)distance);
            data_struct.data.push_back((float)angle);
            break;
        }

        default:
        std::cout << "未知数据类型" << std::endl;
        return false;
    }

    data_queue.push_back(data_struct);

    return true;
}

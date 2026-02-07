/**
 * @file data_processing.cpp
 * @brief 无人机数据处理模块实现文件
 * @details 该文件实现了无人机数据的多格式解析功能，支持字符串、JSON和二进制格式
 * @author DroneSwarm Team
 * @date 2025
 */

#include "data_processing.h"

/**
 * @brief 解析字符串格式的无人机数据
 * @param data 输入的字符串数据，格式为 "key1=value1,key2=value2,..."
 * @details 支持的数据字段：
 *          - id: 无人机编号
 *          - roll, pitch, yaw: MPU6050姿态数据
 *          - x, y, z: GPS位置数据
 *          - batt: 电池电压数据
 *          - pid0_kp~pid3_kd: 四个电机的PID参数
 * @note 输入字符串应遵循 key=value 的格式，多个键值对用逗号分隔
 * @example "id=1,x=100,y=200,z=50,batt=12,pid0_kp=10"
 */
void DataProcessing::ParseData(const std::string& data)
{
    // 使用字符串流进行解析，按逗号分割每个键值对
    std::stringstream ss(data);
    std::string item;
    
    // 逐个解析每个键值对
    while (std::getline(ss, item, ','))
    {
        // 查找等号位置，分离键和值
        size_t pos = item.find('=');
        if (pos != std::string::npos)
        {
            std::string key = item.substr(0, pos);      // 提取键名
            std::string value = item.substr(pos + 1);   // 提取键值
            
            try {
                // 根据键名设置对应的数据成员
                if (key == "id") {
                    id = static_cast<uint8_t>(std::stoi(value));    // 无人机编号
                }
                // MPU6050姿态数据解析
                else if (key == "roll") {
                    roll = static_cast<int16_t>(std::stoi(value));  // 横滚角
                }
                else if (key == "pitch") {
                    pitch = static_cast<int16_t>(std::stoi(value)); // 俯仰角
                }
                else if (key == "yaw") {
                    yaw = static_cast<int16_t>(std::stoi(value));   // 偏航角
                }
                // GPS位置数据解析
                else if (key == "x") {
                    x = static_cast<float>(std::stoi(value));     // 经度
                }
                else if (key == "y") {
                    y = static_cast<float>(std::stoi(value));     // 纬度
                }
                else if (key == "z") {
                    z = static_cast<float>(std::stoi(value));     // 海拔高度
                }
                // 电池电压数据解析
                else if (key == "batt") {
                    batt = static_cast<uint8_t>(std::stoi(value));  // 电池电压
                }
                // 一号电机PID参数解析
                else if (key == "pid0_kp") {
                    pid[0].kp = std::stof(value); // 比例系数
                }
                else if (key == "pid0_ki") {
                    pid[0].ki = std::stof(value); // 积分系数
                }
                else if (key == "pid0_kd") {
                    pid[0].kd = std::stof(value); // 微分系数
                }
                // 二号电机PID参数解析
                else if (key == "pid1_kp") {
                    pid[1].kp = std::stof(value);
                }
                else if (key == "pid1_ki") {
                    pid[1].ki = std::stof(value);
                }
                else if (key == "pid1_kd") {
                    pid[1].kd = std::stof(value);
                }
                // 三号电机PID参数解析
                else if (key == "pid2_kp") {
                    pid[2].kp = std::stof(value);
                }
                else if (key == "pid2_ki") {
                    pid[2].ki = std::stof(value);
                }
                else if (key == "pid2_kd") {
                    pid[2].kd = std::stof(value);
                }
                // 四号电机PID参数解析
                else if (key == "pid3_kp") {
                    pid[3].kp = std::stof(value);
                }
                else if (key == "pid3_ki") {
                    pid[3].ki = std::stof(value);
                }
                else if (key == "pid3_kd") {
                    pid[3].kd = std::stof(value);
                }
                // 未知键名则忽略
            }
            catch (const std::exception& e) {
                // 数值转换失败，抛出异常给上层处理
                throw std::runtime_error("字符串解析失败，键[" + key + "]的值[" + value + "]无效: " + e.what());
            }
        }
    }
}
/**
 * @brief 解析JSON格式的无人机数据
 * @param data 输入的JSON对象，包含无人机各项数据
 * @details 支持两种PID数据格式：
 *          1. 数组格式: "pid": [{"kp":1,"ki":2,"kd":3}, ...]
 *          2. 键值对格式: "pid0_kp":1, "pid0_ki":2, "pid0_kd":3, ...
 * @note 所有字段都是可选的，函数会检查字段存在性和数据类型
 * @example 
 *   {
 *     "id": 1,
 *     "roll": 10, "pitch": 5, "yaw": 180,
 *     "x": 100, "y": 200, "z": 50,
 *     "batt": 12,
 *     "pid": [{"kp":10,"ki":5,"kd":2}, {"kp":8,"ki":4,"kd":1}]
 *   }
 */
void DataProcessing::ParseData(const Json::Value& data)
{
    // 解析无人机编号，检查字段存在性和数据类型
    if (data.isMember("id") && data["id"].isInt()) {
        id = static_cast<uint8_t>(data["id"].asInt());
    }
    
    // 解析MPU6050姿态数据
    if (data.isMember("roll") && data["roll"].isInt()) {
        roll = static_cast<int16_t>(data["roll"].asInt());      // 横滚角
    }
    if (data.isMember("pitch") && data["pitch"].isInt()) {
        pitch = static_cast<int16_t>(data["pitch"].asInt());    // 俯仰角
    }
    if (data.isMember("yaw") && data["yaw"].isInt()) {
        yaw = static_cast<int16_t>(data["yaw"].asInt());        // 偏航角
    }
    
    // 解析GPS位置数据
    if (data.isMember("x") && (data["x"].isNumeric() || data["x"].isInt())) {
        x = static_cast<float>(data["x"].asDouble());            // 经度
    }
    if (data.isMember("y") && (data["y"].isNumeric() || data["y"].isInt())) {
        y = static_cast<float>(data["y"].asDouble());            // 纬度
    }
    if (data.isMember("z") && (data["z"].isNumeric() || data["z"].isInt())) {
        z = static_cast<float>(data["z"].asDouble());            // 海拔高度
    }
    
    // 解析电池电压数据
    if (data.isMember("batt") && data["batt"].isInt()) {
        batt = static_cast<uint8_t>(data["batt"].asInt());      // 电池电压
    }
    
    // 解析PID数据 - 优先处理数组格式
    if (data.isMember("pid") && data["pid"].isArray()) {
        const Json::Value& pid_array = data["pid"];
        // 遍历PID数组，最多处理4个电机的PID参数
        for (Json::ArrayIndex i = 0; i < std::min(static_cast<Json::ArrayIndex>(4), pid_array.size()); i++) {
            if (pid_array[i].isObject()) {
                // 解析第i个电机的PID参数（支持int和float）
                if (pid_array[i].isMember("kp") && pid_array[i]["kp"].isNumeric()) {
                    pid[i].kp = static_cast<float>(pid_array[i]["kp"].asDouble());   // 比例系数
                }
                if (pid_array[i].isMember("ki") && pid_array[i]["ki"].isNumeric()) {
                    pid[i].ki = static_cast<float>(pid_array[i]["ki"].asDouble());   // 积分系数
                }
                if (pid_array[i].isMember("kd") && pid_array[i]["kd"].isNumeric()) {
                    pid[i].kd = static_cast<float>(pid_array[i]["kd"].asDouble());   // 微分系数
                }
            }
        }
    }
    else {
        // 如果没有数组形式，则检查键值对形式的PID数据
        for (int i = 0; i < 4; i++) {
            std::string prefix = "pid" + std::to_string(i) + "_";
            // 解析第i个电机的PID参数（支持int和float）
            if (data.isMember(prefix + "kp") && data[prefix + "kp"].isNumeric()) {
                pid[i].kp = static_cast<float>(data[prefix + "kp"].asDouble());
            }
            if (data.isMember(prefix + "ki") && data[prefix + "ki"].isNumeric()) {
                pid[i].ki = static_cast<float>(data[prefix + "ki"].asDouble());
            }
            if (data.isMember(prefix + "kd") && data[prefix + "kd"].isNumeric()) {
                pid[i].kd = static_cast<float>(data[prefix + "kd"].asDouble());
            }
        }
    }
}

/**
 * @brief 解析二进制格式的无人机数据包
 * @param data 输入的二进制数据指针
 * @details 数据包格式：
 *   [包头0][包头1][ID位][状态位][数据长度][参数位1][参数位2]...[参数位n][校验位][包尾]
 *    0xEE   0xEE    id    status   length    param1    param2      paramn   checksum 0xFF
 * 
 * @note 状态位定义：
 *   - 0x00: 姿态数据 (roll, pitch, yaw) - 6字节
 *   - 0x01: GPS数据 (x, y, z) - 12字节  
 *   - 0x02: ADC数据 (电池电压) - 1字节
 *   - 0x03: 无人机编号 - 1字节
 *   - 0x04~0x07: 电机PID数据 (kp, ki, kd) - 12字节（每个参数4字节float）
 * 
 * @note 校验算法：（ID不参与验算）
 *   校验位 = ( 状态位 + 数据长度 + 参数位1 + 参数位2 + ... + 参数位n) & 0xFF
 * 
 * @example GPS数据包 (16位格式):
 *   [0xEE][0xEE][1][0x01][0x06][0x00][100][0x00][200][0x00][50][checksum][0xFF]
 *   表示ID=1的无人机GPS坐标 x=100, y=200, z=50 (每个坐标16位，length=6)
 *
 * @example 姿态数据包 (16位格式):
 *   [0xEE][0xEE][2][0x00][0x06][0x00][10][0xFF][350][0x01][80][checksum][0xFF]  
 *   表示ID=2的无人机姿态 roll=10, pitch=-6, yaw=336 (每个角度16位，length=6)
 */
void DataProcessing::ParseData(const uint8_t* data)
{
    uint8_t check = 0;  // 校验和计算变量
    
    // 验证包头是否正确 (0xEE 0xEE) 或者数据为空
    if (data == nullptr || data[0] != 0xEE || data[1] != 0xEE)
    {
        return; // 包头错误，丢弃数据包
    }

    uint8_t status = data[3];       // 数据类型标识
    uint8_t length = data[4];       // 参数数据长度

    check +=  status + length;

    // 计算校验和：累加所有参数位
    for (int i = 0; i < length; i++)
    {
        check += data[5 + i];
    }
    
    
    // 验证校验位和包尾
    if (data[5 + length] != (check & 0xFF) || data[6 + length] != 0xFF)
    {
        return; // 校验失败或包尾错误，丢弃数据包
    }

    // 根据状态位解析不同类型的数据
    switch (status)
    {
        case 0x00: // 姿态数据解析
            // 最高位是符号位
            roll = data[5] << 8 | data[6]; // 横滚角 高八位+低八位
            pitch = data[7] << 8 | data[8]; // 俯仰角 高八位+低八位
            yaw = data[9] << 8 | data[10]; // 偏航角 高八位+低八位
            break;
            
        case 0x01: // GPS位置数据解析
            // 注意：GPS数据为浮点数，一共四个字节
            x = data[5] << 24 | data[6] << 16 | data[7] << 8 | data[8];    
            y = data[9] << 24 | data[10] << 16 | data[11] << 8 | data[12];    
            z = data[13] << 24 | data[14] << 16 | data[15] << 8 | data[16];    
            break;
            
        case 0x02: // 电池电压数据解析
            batt = data[5];
            break;
            
        case 0x03: // 无人机编号解析
            id = data[5];
            break;
            
        case 0x04: // 一号电机PID参数解析（每个参数4字节float）
            {
                uint32_t kp_bits = data[5] << 24 | data[6] << 16 | data[7] << 8 | data[8];
                uint32_t ki_bits = data[9] << 24 | data[10] << 16 | data[11] << 8 | data[12];
                uint32_t kd_bits = data[13] << 24 | data[14] << 16 | data[15] << 8 | data[16];
                pid[0].kp = *reinterpret_cast<float*>(&kp_bits);
                pid[0].ki = *reinterpret_cast<float*>(&ki_bits);
                pid[0].kd = *reinterpret_cast<float*>(&kd_bits);
            }
            break;
            
        case 0x05: // 二号电机PID参数解析（每个参数4字节float）
            {
                uint32_t kp_bits = data[5] << 24 | data[6] << 16 | data[7] << 8 | data[8];
                uint32_t ki_bits = data[9] << 24 | data[10] << 16 | data[11] << 8 | data[12];
                uint32_t kd_bits = data[13] << 24 | data[14] << 16 | data[15] << 8 | data[16];
                pid[1].kp = *reinterpret_cast<float*>(&kp_bits);
                pid[1].ki = *reinterpret_cast<float*>(&ki_bits);
                pid[1].kd = *reinterpret_cast<float*>(&kd_bits);
            }
            break;
            
        case 0x06: // 三号电机PID参数解析（每个参数4字节float）
            {
                uint32_t kp_bits = data[5] << 24 | data[6] << 16 | data[7] << 8 | data[8];
                uint32_t ki_bits = data[9] << 24 | data[10] << 16 | data[11] << 8 | data[12];
                uint32_t kd_bits = data[13] << 24 | data[14] << 16 | data[15] << 8 | data[16];
                pid[2].kp = *reinterpret_cast<float*>(&kp_bits);
                pid[2].ki = *reinterpret_cast<float*>(&ki_bits);
                pid[2].kd = *reinterpret_cast<float*>(&kd_bits);
            }
            break;
            
        case 0x07: // 四号电机PID参数解析（每个参数4字节float）
            {
                uint32_t kp_bits = data[5] << 24 | data[6] << 16 | data[7] << 8 | data[8];
                uint32_t ki_bits = data[9] << 24 | data[10] << 16 | data[11] << 8 | data[12];
                uint32_t kd_bits = data[13] << 24 | data[14] << 16 | data[15] << 8 | data[16];
                pid[3].kp = *reinterpret_cast<float*>(&kp_bits);
                pid[3].ki = *reinterpret_cast<float*>(&ki_bits);
                pid[3].kd = *reinterpret_cast<float*>(&kd_bits);
            }
            break;
        case 0xFF: // 获取所有数据（68字节）
           {
                id = data[5];
                roll = data[6] << 8 | data[7];
                pitch = data[8] << 8 | data[9];
                yaw = data[10] << 8 | data[11];
                
                uint32_t x_bits = data[12] << 24 | data[13] << 16 | data[14] << 8 | data[15];
                uint32_t y_bits = data[16] << 24 | data[17] << 16 | data[18] << 8 | data[19];
                uint32_t z_bits = data[20] << 24 | data[21] << 16 | data[22] << 8 | data[23];
                x = *reinterpret_cast<float*>(&x_bits);
                y = *reinterpret_cast<float*>(&y_bits);
                z = *reinterpret_cast<float*>(&z_bits);
                
                batt = data[24];
                
                // PID数据（每个参数4字节float，共48字节）
                uint32_t pid0_kp_bits = data[25] << 24 | data[26] << 16 | data[27] << 8 | data[28];
                uint32_t pid0_ki_bits = data[29] << 24 | data[30] << 16 | data[31] << 8 | data[32];
                uint32_t pid0_kd_bits = data[33] << 24 | data[34] << 16 | data[35] << 8 | data[36];
                pid[0].kp = *reinterpret_cast<float*>(&pid0_kp_bits);
                pid[0].ki = *reinterpret_cast<float*>(&pid0_ki_bits);
                pid[0].kd = *reinterpret_cast<float*>(&pid0_kd_bits);
                
                uint32_t pid1_kp_bits = data[37] << 24 | data[38] << 16 | data[39] << 8 | data[40];
                uint32_t pid1_ki_bits = data[41] << 24 | data[42] << 16 | data[43] << 8 | data[44];
                uint32_t pid1_kd_bits = data[45] << 24 | data[46] << 16 | data[47] << 8 | data[48];
                pid[1].kp = *reinterpret_cast<float*>(&pid1_kp_bits);
                pid[1].ki = *reinterpret_cast<float*>(&pid1_ki_bits);
                pid[1].kd = *reinterpret_cast<float*>(&pid1_kd_bits);
                
                uint32_t pid2_kp_bits = data[49] << 24 | data[50] << 16 | data[51] << 8 | data[52];
                uint32_t pid2_ki_bits = data[53] << 24 | data[54] << 16 | data[55] << 8 | data[56];
                uint32_t pid2_kd_bits = data[57] << 24 | data[58] << 16 | data[59] << 8 | data[60];
                pid[2].kp = *reinterpret_cast<float*>(&pid2_kp_bits);
                pid[2].ki = *reinterpret_cast<float*>(&pid2_ki_bits);
                pid[2].kd = *reinterpret_cast<float*>(&pid2_kd_bits);
                
                uint32_t pid3_kp_bits = data[61] << 24 | data[62] << 16 | data[63] << 8 | data[64];
                uint32_t pid3_ki_bits = data[65] << 24 | data[66] << 16 | data[67] << 8 | data[68];
                uint32_t pid3_kd_bits = data[69] << 24 | data[70] << 16 | data[71] << 8 | data[72];
                pid[3].kp = *reinterpret_cast<float*>(&pid3_kp_bits);
                pid[3].ki = *reinterpret_cast<float*>(&pid3_ki_bits);
                pid[3].kd = *reinterpret_cast<float*>(&pid3_kd_bits);
            break;
           }
            
        default: // 未知状态位，忽略数据包
            break;
    }
}

/**
 * @brief 解析vector<uint8_t>格式的无人机数据包
 * @param data 输入的vector<uint8_t>数据
 * @details 这是一个适配器方法，将vector转换为指针后调用二进制解析方法
 */
void DataProcessing::ParseData(const std::vector<uint8_t>& data)
{
    if (!data.empty()) {
        ParseData(data.data());  // 调用已有的二进制数据解析方法
    }
}

// 数据格式
// IP和端口 的数组
void DataProcessing::Init_ParseData(const std::vector<Json::Value>& data)
{
    for (auto &item : data)
    {
        if (item.isMember("ip") && item["ip"].isString() && item.isMember("port") && item["port"].isInt())
        {
            std::string ip = item["ip"].asString();
            int port = item["port"].asInt();
            swarm_registry.registerDrone(ip, port);
        }
    }
}

#include "ROS_DATA_TX.h"

// 新构造函数实现（使用UDP实例）
ROS_DATA_TX::ROS_DATA_TX(UDP* udp, uint8_t drone_id)
    : send_u8_array(nullptr), udp_instance(udp), id(drone_id)
{
}

// 内部发送函数（根据初始化方式选择发送方法）
bool send_data_internal(UDP* udp_instance, send_u8_array_t send_func, uint8_t* data, uint8_t length)
{
    if (udp_instance != nullptr) {
        return udp_instance->send_data(data, length);
    } else if (send_func != nullptr) {
        return send_func(data, length);
    }
    return false;
}

uint8_t ROS_DATA_TX::parse_checksum(uint8_t length, uint8_t* data)
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

// 发送姿态数据
/**
 * @details 数据包格式：
 *   [包头0][包头1][ID位][状态位][数据长度][参数位1] [参数位2]   ...   [参数位n][校验位][包尾]
 *   [0xEE] [0xEE] [ID位] [0x00]  [0x06]  [roll高8位][roll低8位][pitch高8位][pitch低8位][yaw高8位][yaw低8位] [校验位] [0xFF]
 */
bool ROS_DATA_TX::posture(float roll, float pitch, float yaw)
{
    uint8_t data[14] = {0xEE, 0xEE, id, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};

    // 将float转换为int16_t再拆分为字节
    int16_t roll_int = static_cast<int16_t>(roll);
    int16_t pitch_int = static_cast<int16_t>(pitch);
    int16_t yaw_int = static_cast<int16_t>(yaw);
    
    data[5] = (roll_int >> 8) & 0xFF;
    data[6] = roll_int & 0xFF;
    data[7] = (pitch_int >> 8) & 0xFF;
    data[8] = pitch_int & 0xFF;
    data[9] = (yaw_int >> 8) & 0xFF;
    data[10] = yaw_int & 0xFF;
    data[11] = parse_checksum(11, data);
    data[12] = 0xFF;
    return send_data_internal(udp_instance, send_u8_array, data, 13);
}


// 发送GPS位置数据
/**
 * @details 数据包格式：
 *   [包头0][包头1][ID位][状态位][数据长度][x高字节]...[x低字节][y高字节]...[y低字节][z高字节]...[z低字节][校验位][包尾]
 *   [0xEE] [0xEE] [ID位] [0x01]  [0x0C]  [x3][x2][x1][x0] [y3][y2][y1][y0] [z3][z2][z1][z0] [校验位] [0xFF]
 */
bool ROS_DATA_TX::gps(float x, float y, float z)
{
    uint8_t data[19] = {0xEE, 0xEE, id, 0x01, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};

    // 将float转换为uint32_t（保持二进制表示）
    uint32_t x_bits = *reinterpret_cast<uint32_t*>(&x);
    uint32_t y_bits = *reinterpret_cast<uint32_t*>(&y);
    uint32_t z_bits = *reinterpret_cast<uint32_t*>(&z);
    
    // X坐标（4字节）
    data[5] = (x_bits >> 24) & 0xFF;
    data[6] = (x_bits >> 16) & 0xFF;
    data[7] = (x_bits >> 8) & 0xFF;
    data[8] = x_bits & 0xFF;
    
    // Y坐标（4字节）
    data[9] = (y_bits >> 24) & 0xFF;
    data[10] = (y_bits >> 16) & 0xFF;
    data[11] = (y_bits >> 8) & 0xFF;
    data[12] = y_bits & 0xFF;
    
    // Z坐标（4字节）
    data[13] = (z_bits >> 24) & 0xFF;
    data[14] = (z_bits >> 16) & 0xFF;
    data[15] = (z_bits >> 8) & 0xFF;
    data[16] = z_bits & 0xFF;
    
    data[17] = parse_checksum(17, data);
    data[18] = 0xFF;
    return send_data_internal(udp_instance, send_u8_array, data, 19);
}


// 发送电池电压数据
/**
 * @details 数据包格式：
 *   [包头0][包头1][ID位][状态位][数据长度][电池电压][校验位][包尾]
 *   [0xEE] [0xEE] [ID位] [0x02]  [0x01]    [batt]   [校验位] [0xFF]
 */
bool ROS_DATA_TX::battery(uint8_t batt)
{
    uint8_t data[8] = {0xEE, 0xEE, id, 0x02, 0x01, 0x00, 0x00, 0xFF};
    data[5] = batt;
    data[6] = parse_checksum(6, data);
    data[7] = 0xFF;
    return send_data_internal(udp_instance, send_u8_array, data, 8);
}

// 发送无人机ID
/**
 * @details 数据包格式：
 *   [包头0][包头1][ID位][状态位][数据长度][ID值][校验位][包尾]
 *   [0xEE] [0xEE] [ID位] [0x03]  [0x01]   [id]  [校验位] [0xFF]
 */
bool ROS_DATA_TX::send_id(uint8_t drone_id)
{
    uint8_t data[8] = {0xEE, 0xEE, id, 0x03, 0x01, 0x00, 0x00, 0xFF};
    data[5] = drone_id;
    data[6] = parse_checksum(6, data);
    data[7] = 0xFF;
    return send_data_internal(udp_instance, send_u8_array, data, 8);
}

// 发送一号电机PID参数
/**
 * @details 数据包格式：
 *   [包头0][包头1][ID位][状态位][数据长度][kp(4字节)][ki(4字节)][kd(4字节)][校验位][包尾]
 *   [0xEE] [0xEE] [ID位] [0x04]  [0x0C]   [kp][ki][kd][校验位] [0xFF]
 */
bool ROS_DATA_TX::pid_motor_1(float kp, float ki, float kd)
{
    uint8_t data[20] = {0xEE, 0xEE, id, 0x04, 0x0C};
    
    // 转换float为字节
    uint32_t kp_bits = *reinterpret_cast<uint32_t*>(&kp);
    uint32_t ki_bits = *reinterpret_cast<uint32_t*>(&ki);
    uint32_t kd_bits = *reinterpret_cast<uint32_t*>(&kd);
    
    data[5] = (kp_bits >> 24) & 0xFF;
    data[6] = (kp_bits >> 16) & 0xFF;
    data[7] = (kp_bits >> 8) & 0xFF;
    data[8] = kp_bits & 0xFF;
    
    data[9] = (ki_bits >> 24) & 0xFF;
    data[10] = (ki_bits >> 16) & 0xFF;
    data[11] = (ki_bits >> 8) & 0xFF;
    data[12] = ki_bits & 0xFF;
    
    data[13] = (kd_bits >> 24) & 0xFF;
    data[14] = (kd_bits >> 16) & 0xFF;
    data[15] = (kd_bits >> 8) & 0xFF;
    data[16] = kd_bits & 0xFF;
    
    data[17] = parse_checksum(17, data);
    data[18] = 0xFF;
    return send_data_internal(udp_instance, send_u8_array, data, 19);
}

// 发送二号电机PID参数
/**
 * @details 数据包格式：
 *   [包头0][包头1][ID位][状态位][数据长度][kp(4字节)][ki(4字节)][kd(4字节)][校验位][包尾]
 *   [0xEE] [0xEE] [ID位] [0x05]  [0x0C]   [kp][ki][kd][校验位] [0xFF]
 */
bool ROS_DATA_TX::pid_motor_2(float kp, float ki, float kd)
{
    uint8_t data[20] = {0xEE, 0xEE, id, 0x05, 0x0C};
    
    uint32_t kp_bits = *reinterpret_cast<uint32_t*>(&kp);
    uint32_t ki_bits = *reinterpret_cast<uint32_t*>(&ki);
    uint32_t kd_bits = *reinterpret_cast<uint32_t*>(&kd);
    
    data[5] = (kp_bits >> 24) & 0xFF;
    data[6] = (kp_bits >> 16) & 0xFF;
    data[7] = (kp_bits >> 8) & 0xFF;
    data[8] = kp_bits & 0xFF;
    
    data[9] = (ki_bits >> 24) & 0xFF;
    data[10] = (ki_bits >> 16) & 0xFF;
    data[11] = (ki_bits >> 8) & 0xFF;
    data[12] = ki_bits & 0xFF;
    
    data[13] = (kd_bits >> 24) & 0xFF;
    data[14] = (kd_bits >> 16) & 0xFF;
    data[15] = (kd_bits >> 8) & 0xFF;
    data[16] = kd_bits & 0xFF;
    
    data[17] = parse_checksum(17, data);
    data[18] = 0xFF;
    return send_data_internal(udp_instance, send_u8_array, data, 19);
}

// 发送三号电机PID参数
/**
 * @details 数据包格式：
 *   [包头0][包头1][ID位][状态位][数据长度][kp(4字节)][ki(4字节)][kd(4字节)][校验位][包尾]
 *   [0xEE] [0xEE] [ID位] [0x06]  [0x0C]   [kp][ki][kd][校验位] [0xFF]
 */
bool ROS_DATA_TX::pid_motor_3(float kp, float ki, float kd)
{
    uint8_t data[20] = {0xEE, 0xEE, id, 0x06, 0x0C};
    
    uint32_t kp_bits = *reinterpret_cast<uint32_t*>(&kp);
    uint32_t ki_bits = *reinterpret_cast<uint32_t*>(&ki);
    uint32_t kd_bits = *reinterpret_cast<uint32_t*>(&kd);
    
    data[5] = (kp_bits >> 24) & 0xFF;
    data[6] = (kp_bits >> 16) & 0xFF;
    data[7] = (kp_bits >> 8) & 0xFF;
    data[8] = kp_bits & 0xFF;
    
    data[9] = (ki_bits >> 24) & 0xFF;
    data[10] = (ki_bits >> 16) & 0xFF;
    data[11] = (ki_bits >> 8) & 0xFF;
    data[12] = ki_bits & 0xFF;
    
    data[13] = (kd_bits >> 24) & 0xFF;
    data[14] = (kd_bits >> 16) & 0xFF;
    data[15] = (kd_bits >> 8) & 0xFF;
    data[16] = kd_bits & 0xFF;
    
    data[17] = parse_checksum(17, data);
    data[18] = 0xFF;
    return send_data_internal(udp_instance, send_u8_array, data, 19);
}

// 发送四号电机PID参数
/**
 * @details 数据包格式：
 *   [包头0][包头1][ID位][状态位][数据长度][kp(4字节)][ki(4字节)][kd(4字节)][校验位][包尾]
 *   [0xEE] [0xEE] [ID位] [0x07]  [0x0C]   [kp][ki][kd][校验位] [0xFF]
 */
bool ROS_DATA_TX::pid_motor_4(float kp, float ki, float kd)
{
    uint8_t data[20] = {0xEE, 0xEE, id, 0x07, 0x0C};
    
    uint32_t kp_bits = *reinterpret_cast<uint32_t*>(&kp);
    uint32_t ki_bits = *reinterpret_cast<uint32_t*>(&ki);
    uint32_t kd_bits = *reinterpret_cast<uint32_t*>(&kd);
    
    data[5] = (kp_bits >> 24) & 0xFF;
    data[6] = (kp_bits >> 16) & 0xFF;
    data[7] = (kp_bits >> 8) & 0xFF;
    data[8] = kp_bits & 0xFF;
    
    data[9] = (ki_bits >> 24) & 0xFF;
    data[10] = (ki_bits >> 16) & 0xFF;
    data[11] = (ki_bits >> 8) & 0xFF;
    data[12] = ki_bits & 0xFF;
    
    data[13] = (kd_bits >> 24) & 0xFF;
    data[14] = (kd_bits >> 16) & 0xFF;
    data[15] = (kd_bits >> 8) & 0xFF;
    data[16] = kd_bits & 0xFF;
    
    data[17] = parse_checksum(17, data);
    data[18] = 0xFF;
    return send_data_internal(udp_instance, send_u8_array, data, 19);
}

// 发送所有数据
/**
 * @details 数据包格式：
 *   [包头0][包头1][ID位][状态位][数据长度][所有参数...][校验位][包尾]
 *   [0xEE] [0xEE] [ID位] [0xFF]  [0x44]   [id][roll][pitch][yaw][x][y][z][batt][pid0-3][校验位] [0xFF]
 * @note 数据长度0x44 = 68字节
 *   - id: 1字节
 *   - roll, pitch, yaw: 各2字节（共6字节）
 *   - x, y, z: 各4字节（共12字节）
 *   - batt: 1字节
 *   - pid[0-3]: 4*3*4=48字节（每个参数4字节float）
 */
bool ROS_DATA_TX::all(uint8_t drone_id, float roll_val, float pitch_val, float yaw_val, 
                      float x_val, float y_val, float z_val, uint8_t batt_val,
                      float pid0_kp, float pid0_ki, float pid0_kd,
                      float pid1_kp, float pid1_ki, float pid1_kd,
                      float pid2_kp, float pid2_ki, float pid2_kd,
                      float pid3_kp, float pid3_ki, float pid3_kd)
{
    uint8_t data[76] = {0xEE, 0xEE, id, 0xFF, 0x44}; // 包头+ID+状态+长度
    
    // ID（1字节）
    data[5] = drone_id;
    
    // 姿态数据（6字节）
    int16_t roll_int = static_cast<int16_t>(roll_val);
    int16_t pitch_int = static_cast<int16_t>(pitch_val);
    int16_t yaw_int = static_cast<int16_t>(yaw_val);
    
    data[6] = (roll_int >> 8) & 0xFF;
    data[7] = roll_int & 0xFF;
    data[8] = (pitch_int >> 8) & 0xFF;
    data[9] = pitch_int & 0xFF;
    data[10] = (yaw_int >> 8) & 0xFF;
    data[11] = yaw_int & 0xFF;
    
    // GPS数据（12字节）
    uint32_t x_bits = *reinterpret_cast<uint32_t*>(&x_val);
    uint32_t y_bits = *reinterpret_cast<uint32_t*>(&y_val);
    uint32_t z_bits = *reinterpret_cast<uint32_t*>(&z_val);
    
    data[12] = (x_bits >> 24) & 0xFF;
    data[13] = (x_bits >> 16) & 0xFF;
    data[14] = (x_bits >> 8) & 0xFF;
    data[15] = x_bits & 0xFF;
    
    data[16] = (y_bits >> 24) & 0xFF;
    data[17] = (y_bits >> 16) & 0xFF;
    data[18] = (y_bits >> 8) & 0xFF;
    data[19] = y_bits & 0xFF;
    
    data[20] = (z_bits >> 24) & 0xFF;
    data[21] = (z_bits >> 16) & 0xFF;
    data[22] = (z_bits >> 8) & 0xFF;
    data[23] = z_bits & 0xFF;
    
    // 电池电压（1字节）
    data[24] = batt_val;
    
    // PID参数（48字节 - 每个参数4字节float）
    uint32_t pid0_kp_bits = *reinterpret_cast<uint32_t*>(&pid0_kp);
    uint32_t pid0_ki_bits = *reinterpret_cast<uint32_t*>(&pid0_ki);
    uint32_t pid0_kd_bits = *reinterpret_cast<uint32_t*>(&pid0_kd);
    
    data[25] = (pid0_kp_bits >> 24) & 0xFF;
    data[26] = (pid0_kp_bits >> 16) & 0xFF;
    data[27] = (pid0_kp_bits >> 8) & 0xFF;
    data[28] = pid0_kp_bits & 0xFF;
    
    data[29] = (pid0_ki_bits >> 24) & 0xFF;
    data[30] = (pid0_ki_bits >> 16) & 0xFF;
    data[31] = (pid0_ki_bits >> 8) & 0xFF;
    data[32] = pid0_ki_bits & 0xFF;
    
    data[33] = (pid0_kd_bits >> 24) & 0xFF;
    data[34] = (pid0_kd_bits >> 16) & 0xFF;
    data[35] = (pid0_kd_bits >> 8) & 0xFF;
    data[36] = pid0_kd_bits & 0xFF;
    
    uint32_t pid1_kp_bits = *reinterpret_cast<uint32_t*>(&pid1_kp);
    uint32_t pid1_ki_bits = *reinterpret_cast<uint32_t*>(&pid1_ki);
    uint32_t pid1_kd_bits = *reinterpret_cast<uint32_t*>(&pid1_kd);
    
    data[37] = (pid1_kp_bits >> 24) & 0xFF;
    data[38] = (pid1_kp_bits >> 16) & 0xFF;
    data[39] = (pid1_kp_bits >> 8) & 0xFF;
    data[40] = pid1_kp_bits & 0xFF;
    
    data[41] = (pid1_ki_bits >> 24) & 0xFF;
    data[42] = (pid1_ki_bits >> 16) & 0xFF;
    data[43] = (pid1_ki_bits >> 8) & 0xFF;
    data[44] = pid1_ki_bits & 0xFF;
    
    data[45] = (pid1_kd_bits >> 24) & 0xFF;
    data[46] = (pid1_kd_bits >> 16) & 0xFF;
    data[47] = (pid1_kd_bits >> 8) & 0xFF;
    data[48] = pid1_kd_bits & 0xFF;
    
    uint32_t pid2_kp_bits = *reinterpret_cast<uint32_t*>(&pid2_kp);
    uint32_t pid2_ki_bits = *reinterpret_cast<uint32_t*>(&pid2_ki);
    uint32_t pid2_kd_bits = *reinterpret_cast<uint32_t*>(&pid2_kd);
    
    data[49] = (pid2_kp_bits >> 24) & 0xFF;
    data[50] = (pid2_kp_bits >> 16) & 0xFF;
    data[51] = (pid2_kp_bits >> 8) & 0xFF;
    data[52] = pid2_kp_bits & 0xFF;
    
    data[53] = (pid2_ki_bits >> 24) & 0xFF;
    data[54] = (pid2_ki_bits >> 16) & 0xFF;
    data[55] = (pid2_ki_bits >> 8) & 0xFF;
    data[56] = pid2_ki_bits & 0xFF;
    
    data[57] = (pid2_kd_bits >> 24) & 0xFF;
    data[58] = (pid2_kd_bits >> 16) & 0xFF;
    data[59] = (pid2_kd_bits >> 8) & 0xFF;
    data[60] = pid2_kd_bits & 0xFF;
    
    uint32_t pid3_kp_bits = *reinterpret_cast<uint32_t*>(&pid3_kp);
    uint32_t pid3_ki_bits = *reinterpret_cast<uint32_t*>(&pid3_ki);
    uint32_t pid3_kd_bits = *reinterpret_cast<uint32_t*>(&pid3_kd);
    
    data[61] = (pid3_kp_bits >> 24) & 0xFF;
    data[62] = (pid3_kp_bits >> 16) & 0xFF;
    data[63] = (pid3_kp_bits >> 8) & 0xFF;
    data[64] = pid3_kp_bits & 0xFF;
    
    data[65] = (pid3_ki_bits >> 24) & 0xFF;
    data[66] = (pid3_ki_bits >> 16) & 0xFF;
    data[67] = (pid3_ki_bits >> 8) & 0xFF;
    data[68] = pid3_ki_bits & 0xFF;
    
    data[69] = (pid3_kd_bits >> 24) & 0xFF;
    data[70] = (pid3_kd_bits >> 16) & 0xFF;
    data[71] = (pid3_kd_bits >> 8) & 0xFF;
    data[72] = pid3_kd_bits & 0xFF;
    
    // 校验位和包尾
    data[73] = parse_checksum(73, data);
    data[74] = 0xFF;
    
    return send_data_internal(udp_instance, send_u8_array, data, 75);
}
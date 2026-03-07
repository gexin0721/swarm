#include <ros/ros.h>
#include "network/serve.hpp"
#include "tool/Logger.hpp"

int main(int argc, char** argv) {
    ros::init(argc, argv, "message_manager");
    ros::NodeHandle nh;

    // 初始化日志系统
    tool::Logger& logger = tool::Logger::getInstance();
    logger.initDefaultLogFile("./logs", "message_manager.log");
    logger.info("Message Manager node started");

    // TODO: 初始化 MQTT 连接和 ROS 通信

    ros::spin();
    return 0;
}

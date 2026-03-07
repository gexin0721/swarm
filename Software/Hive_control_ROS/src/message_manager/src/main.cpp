#include <ros/ros.h>
#include "network/serve.hpp"
#include "tool/Logger.hpp"

// 声明测试函数 (定义在 test/test_mqtt_client.cpp)
void run_all_tests(const std::string& broker_address,
                   const std::string& username = "",
                   const std::string& password = "");

int main(int argc, char** argv) {
    ros::init(argc, argv, "message_manager");
    ros::NodeHandle nh;

    // 初始化日志系统
    tool::Logger& logger = tool::Logger::getInstance();
    logger.initDefaultLogFile("./logs", "message_manager.log");
    logger.info("Message Manager node started");

    // 运行MQTT客户端测试
    logger.info("Running MQTT client tests...");
    run_all_tests("tcp://localhost:1883");  // 测试本地MQTT服务器
    logger.info("Tests completed");

    return 0;
}

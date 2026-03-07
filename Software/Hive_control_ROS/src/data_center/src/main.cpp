#include <ros/ros.h>
#include "Datacore/Datacore.h"

int main(int argc, char** argv) {
    ros::init(argc, argv, "data_center");
    ros::NodeHandle nh;

    ROS_INFO("Data Center node started");

    // TODO: 初始化数据库连接和 ROS 服务

    ros::spin();
    return 0;
}

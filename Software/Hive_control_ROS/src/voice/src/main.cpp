#include <ros/ros.h>
#include <iostream>
#include "serve/VoiceService.h"

int main(int argc, char** argv) {
    ros::init(argc, argv, "voice_node");
    ros::NodeHandle nh;
    
    std::cout << "Voice System Starting..." << std::endl;
    
    try {
        VoiceService service(nh);
        service.start();
        
        ros::spin();
        
        service.stop();
    } catch (const std::exception& e) {
        ROS_ERROR("Voice system error: %s", e.what());
        return 1;
    }
    
    std::cout << "Voice System Stopped." << std::endl;
    return 0;
}

cmake_minimum_required(VERSION 3.0.2)
project(message_manager)

# Compile as C++17 (required by Paho MQTT C++ library)
add_compile_options(-std=c++17)

#######################
## Find Dependencies ##
#######################

# ROS packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
)

# System dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

###########################
## Catkin Configuration ##
###########################

catkin_package(
  CATKIN_DEPENDS roscpp rospy std_msgs
)

###########
## Build ##
###########

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${JSONCPP_INCLUDE_DIRS}
)

# Executable
add_executable(${PROJECT_NAME}
  src/main.cpp
  src/network/serve.cpp
  src/tool/Logger.cpp
  test/test_mqtt_client.cpp
)

target_link_libraries(${PROJECT_NAME}
  ${catkin_LIBRARIES}
  ${JSONCPP_LIBRARIES}
  paho-mqttpp3
  paho-mqtt3as
  pthread
)

cmake_minimum_required(VERSION 3.0.2)
project(mic)

# Compile as C++11
add_compile_options(-std=c++11)

#######################
## Find Dependencies ##
#######################

# System dependencies
find_package(ALSA REQUIRED)

# ROS packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
)
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
  ${ALSA_INCLUDE_DIRS}
)

# Executables
add_executable(mic src/angle_wakeup/mic.cpp)
# add_executable(tts src/tts/tts_init.cpp)  # 需要 libmsc 库

# Link libraries
target_link_libraries(mic
  ${catkin_LIBRARIES}
  # /usr/lib/libmsc.so  # 科大讯飞语音库（需要安装）
  ${ALSA_LIBRARIES}
  jsoncpp
)

# target_link_libraries(tts
#   ${catkin_LIBRARIES}
#   msc rt dl pthread stdc++
# )

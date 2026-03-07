cmake_minimum_required(VERSION 3.0.2)
project(voice)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
)

# 查找外部依赖
find_package(ALSA REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

catkin_package(
  CATKIN_DEPENDS roscpp rospy std_msgs
)

include_directories(
  ${catkin_INCLUDE_DIRS}
  ${ALSA_INCLUDE_DIRS}
  ${JSONCPP_INCLUDE_DIRS}
  src
)

# 构建主程序
add_executable(voice_node
  src/main.cpp
  src/asr/ASR.cpp
  src/audio/capture/AudioCapture.cpp
  src/audio/process/AudioProcess.cpp
  src/wakeword/WakeWordDetector.cpp
  src/tts/TTS.cpp
  src/serve/VoiceService.cpp
)

target_link_libraries(voice_node
  ${catkin_LIBRARIES}
  ${ALSA_LIBRARIES}
  ${JSONCPP_LIBRARIES}
  # -lfunasr  # FunASR库（安装后取消注释）
)

# 安装Python脚本
catkin_install_python(PROGRAMS
  scripts/test_audio.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

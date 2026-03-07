cmake_minimum_required(VERSION 3.0.2)
project(data_center)

# Compile as C++14 (required by ODB)
add_compile_options(-std=c++14)

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

# ODB configuration
find_program(ODB_COMPILER odb)
if(NOT ODB_COMPILER)
  message(FATAL_ERROR "ODB compiler not found. Please install libodb-dev and odb packages.")
endif()

find_library(ODB_LIB odb)
find_library(ODB_SQLITE_LIB odb-sqlite)
if(NOT ODB_LIB OR NOT ODB_SQLITE_LIB)
  message(FATAL_ERROR "ODB libraries not found. Please install libodb-dev and libodb-sqlite-dev.")
endif()

set(ODB_INCLUDE_DIRS /usr/include)
set(ODB_LIBRARIES ${ODB_LIB} ${ODB_SQLITE_LIB})

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
  include/Datacore
  ${catkin_INCLUDE_DIRS}
  ${JSONCPP_INCLUDE_DIRS}
  ${ODB_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
)

# ODB code generation
set(ODB_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Datacore/models.hxx
)

set(ODB_GENERATED_SOURCES)
foreach(header ${ODB_HEADERS})
  get_filename_component(name ${header} NAME_WE)
  set(generated_cxx ${CMAKE_CURRENT_BINARY_DIR}/${name}-odb.cxx)
  set(generated_hxx ${CMAKE_CURRENT_BINARY_DIR}/${name}-odb.hxx)
  set(generated_ixx ${CMAKE_CURRENT_BINARY_DIR}/${name}-odb.ixx)

  add_custom_command(
    OUTPUT ${generated_cxx} ${generated_hxx} ${generated_ixx}
    COMMAND ${ODB_COMPILER}
    ARGS -d sqlite --std c++14 --generate-query --generate-schema
         -I ${CMAKE_CURRENT_SOURCE_DIR}/include
         -I ${CMAKE_CURRENT_SOURCE_DIR}/include/Datacore
         -o ${CMAKE_CURRENT_BINARY_DIR}
         ${header}
    DEPENDS ${header}
    COMMENT "Generating ODB code for ${name}"
  )

  list(APPEND ODB_GENERATED_SOURCES ${generated_cxx})
endforeach()

# Executable
add_executable(${PROJECT_NAME}
  src/main.cpp
  src/Datacore/Datacore.cpp
  ${ODB_GENERATED_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
  ${catkin_LIBRARIES}
  ${JSONCPP_LIBRARIES}
  ${ODB_LIBRARIES}
)

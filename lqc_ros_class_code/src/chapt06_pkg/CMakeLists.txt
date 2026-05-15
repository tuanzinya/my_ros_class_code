cmake_minimum_required(VERSION 3.0.2)
project(chapt06_pkg)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  cv_bridge
  geometry_msgs
  roscpp
  rospy
  sensor_msgs
  std_msgs
  std_srvs
  tf
  tf2_ros
  angles
  move_base_msgs
)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES chapt06_pkg
#  CATKIN_DEPENDS cv_bridge geometry_msgs roscpp rospy sensor_msgs std_msgs std_srvs tf tf2_ros
#  DEPENDS system_lib
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)

add_executable(ros_imu_node  src/ros_imu.cpp)
target_link_libraries(ros_imu_node  ${catkin_LIBRARIES})

add_executable(ros_imu_rotate_node src/imu_rotate.cpp)
target_link_libraries(ros_imu_rotate_node ${catkin_LIBRARIES})

add_executable(ros_scan_node src/ros_scan.cpp)
target_link_libraries(ros_scan_node ${catkin_LIBRARIES})

add_executable(ros_avoid_node src/ros_avoid.cpp)
target_link_libraries(ros_avoid_node ${catkin_LIBRARIES})

add_executable(movebase_client_node src/movebase_client.cpp)
target_link_libraries(movebase_client_node ${catkin_LIBRARIES})

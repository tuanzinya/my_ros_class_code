cmake_minimum_required(VERSION 3.0.2)
project(visual_indetity)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  geometry_msgs
  roscpp
  rospy
  sensor_msgs
  std_msgs
  std_srvs
  cv_bridge
  tf2_ros
  upros_message
)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES lqc_ros_class_pkg
  CATKIN_DEPENDS roscpp std_msgs sensor_msgs geometry_msgs rospy cv_bridge std_srvs upros_message tf2_ros
#  DEPENDS system_lib
)

add_executable(tag_grab_node src/tag_grab.cpp)
target_link_libraries(tag_grab_node ${catkin_LIBRARIES})

include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)

catkin_install_python(PROGRAMS
  scripts/follow_line.py   # 这里写你的 py 文件名
  scripts/gesture_movement.py
  scripts/get_ros_image.py
  scripts/apriltag_follow.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
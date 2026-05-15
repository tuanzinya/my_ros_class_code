cmake_minimum_required(VERSION 3.0.2)
project(chapt07_pkg)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  sensor_msgs
  std_msgs
  std_srvs
  tf2_ros
  upros_message
  cv_bridge
  geometry_msgs
)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES lqc_ros_class_pkg
CATKIN_DEPENDS roscpp std_msgs sensor_msgs geometry_msgs rospy cv_bridge std_srvs upros_message tf2_ros
#  DEPENDS system_lib
)

add_executable(tag_grab_new_node src/tag_grab.cpp)
target_link_libraries(tag_grab_new_node ${catkin_LIBRARIES})


## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
  ${catkin_INCLUDE_DIRS}
)

add_executable(speech_control_movebase_node src/speech_control_movebase.cpp)
target_link_libraries(speech_control_movebase_node ${catkin_LIBRARIES})

catkin_install_python(PROGRAMS
  scripts/llm_chat.py
  scripts/voice_control.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)


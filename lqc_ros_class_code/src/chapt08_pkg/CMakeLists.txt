cmake_minimum_required(VERSION 3.0.2)
project(chapt08_pkg)

find_package(catkin REQUIRED COMPONENTS
  geometry_msgs
  roscpp
  tf2
  tf2_ros
  move_base_msgs
  actionlib
  std_msgs
  apriltag_ros
  dynamic_reconfigure
)

catkin_package()

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_executable(fucking_run_node src/fucking_run.cpp)
target_link_libraries(fucking_run_node ${catkin_LIBRARIES})

install(DIRECTORY launch config
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)
install(DIRECTORY include/${PROJECT_NAME}/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
)

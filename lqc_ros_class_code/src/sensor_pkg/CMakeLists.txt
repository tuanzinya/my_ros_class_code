cmake_minimum_required(VERSION 3.0.2)
project(sensor_pkg)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  sensor_msgs
  geometry_msgs
  tf
)

catkin_package(
  CATKIN_DEPENDS roscpp std_msgs sensor_msgs geometry_msgs tf
#  DEPENDS system_lib
)

include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)

# add_executable(ros_bump_node src/ros_bump.cpp)
# target_link_libraries(ros_bump_node ${catkin_LIBRARIES})

add_executable(imu_spin_node src/ros_imu.cpp)
target_link_libraries(imu_spin_node ${catkin_LIBRARIES})
add_dependencies(imu_spin_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

add_executable(bump_avoid_node  src/ros_bump_avoid.cpp)
target_link_libraries(bump_avoid_node  ${catkin_LIBRARIES})

add_executable(tof_avoid_node src/ros_tof_avoid.cpp)
target_link_libraries(tof_avoid_node ${catkin_LIBRARIES})

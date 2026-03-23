cmake_minimum_required(VERSION 3.8)
project(my_class_pkg)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(rclpy REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(action_msgs REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/MyMessage.msg"
  "srv/MyServiceMsg.srv"
  "action/MyAction.action"
  DEPENDENCIES action_msgs  
)

add_executable(my_publisher src/ros_publisher.cpp)
ament_target_dependencies(my_publisher rclcpp std_msgs)

add_executable(my_subscriber src/ros_subscriber.cpp)
ament_target_dependencies(my_subscriber rclcpp std_msgs)

add_executable(my_message_publisher src/msg_publisher.cpp)
ament_target_dependencies(my_message_publisher rclcpp std_msgs)

add_executable(my_message_subscriber src/msg_subscriber.cpp)
ament_target_dependencies(my_message_subscriber rclcpp std_msgs)

add_executable(my_service_server src/ros_server.cpp)
ament_target_dependencies(my_service_server rclcpp std_msgs)

add_executable(my_service_client src/ros_client.cpp)
ament_target_dependencies(my_service_client rclcpp std_msgs)

add_executable(my_action_server src/ros_action_server.cpp)
ament_target_dependencies(my_action_server rclcpp std_msgs rclcpp_action)

add_executable(my_action_client src/ros_action_client.cpp)
ament_target_dependencies(my_action_client rclcpp std_msgs rclcpp_action)

rosidl_get_typesupport_target(cpp_typesupport_target ${PROJECT_NAME} "rosidl_typesupport_cpp")
target_link_libraries(my_message_publisher "${cpp_typesupport_target}")
target_link_libraries(my_message_subscriber "${cpp_typesupport_target}")
target_link_libraries(my_service_server "${cpp_typesupport_target}")
target_link_libraries(my_service_client "${cpp_typesupport_target}")
target_link_libraries(my_action_server "${cpp_typesupport_target}")
target_link_libraries(my_action_client "${cpp_typesupport_target}")
  
install(TARGETS
  my_publisher
  my_subscriber
  my_message_publisher
  my_message_subscriber
  my_service_server
  my_service_client
  my_action_client
  my_action_server
  DESTINATION lib/${PROJECT_NAME})

ament_export_dependencies(rosidl_default_runtime)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()

# 安装 launch 文件
install(
  DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

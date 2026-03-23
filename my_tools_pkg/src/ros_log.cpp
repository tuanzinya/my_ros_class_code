#include "rclcpp/rclcpp.hpp"

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("ros2_logging_cpp");

  RCLCPP_DEBUG(node->get_logger(), "This is a DEBUG message.");
  RCLCPP_INFO(node->get_logger(), "This is an INFO message.");
  RCLCPP_WARN(node->get_logger(), "This is a WARNING message.");
  RCLCPP_ERROR(node->get_logger(), "This is an ERROR message.");
  RCLCPP_FATAL(node->get_logger(), "This is a FATAL message.");

  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
#include "rclcpp/rclcpp.hpp"

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("my_param_node");
  std::string param_name = "my_param";
  std::string param_value = "hello";
  std::string my_param_first;

  if (!node->has_parameter(param_name))
    my_param_first = "";
  else
    node->get_parameter(param_name, my_param_first);

  RCLCPP_INFO(node->get_logger(), "Private 1 The value of %s is %s", param_name.c_str(), my_param_first.c_str());
  node->declare_parameter<std::string>(param_name, param_value);
  node->get_parameter(param_name, my_param_first);
  RCLCPP_INFO(node->get_logger(), "Private 2 The value of %s is %s", param_name.c_str(), my_param_first.c_str());
  std::string my_param;
  node->set_parameter(rclcpp::Parameter(param_name, param_value));
  node->get_parameter(param_name, my_param);
  RCLCPP_INFO(node->get_logger(), "The value of %s is %s", param_name.c_str(), my_param.c_str());

  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
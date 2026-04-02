#include "rclcpp/rclcpp.hpp"

int main(int argc, char * argv[])
{
  // 初始化 ROS2
  rclcpp::init(argc, argv);

  // 创建节点
  auto node = std::make_shared<rclcpp::Node>("my_param_node");

  std::string param_name = "my_param";
  std::string param_value = "hello";
  std::string my_param_first;

  // ================ 私有参数第一次获取（未设置） ================
  if (!node->has_parameter(param_name)) {
    my_param_first = "";
  } else {
    node->get_parameter(param_name, my_param_first);
  }
  // 原始英文日志
  RCLCPP_INFO(node->get_logger(), "Private 1 The value of %s is %s", param_name.c_str(), my_param_first.c_str());

  // ================ 设置私有参数 ================
  node->declare_parameter<std::string>(param_name, param_value);

  // ================ 私有参数第二次获取 ================
  node->get_parameter(param_name, my_param_first);
  // 原始英文日志
  RCLCPP_INFO(node->get_logger(), "Private 2 The value of %s is %s", param_name.c_str(), my_param_first.c_str());

  // ================ 设置公有参数 ================
  std::string my_param;
  node->set_parameter(rclcpp::Parameter(param_name, param_value));

  // ================ 获取公有参数 ================
  node->get_parameter(param_name, my_param);
  // 原始英文日志
  RCLCPP_INFO(node->get_logger(), "The value of %s is %s", param_name.c_str(), my_param.c_str());

  // 循环
  rclcpp::spin(node);

  // 关闭
  rclcpp::shutdown();
  return 0;
}
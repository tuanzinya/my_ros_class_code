#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "my_class_pkg/action/my_action.hpp"

using namespace std::placeholders;
using MyAction = my_class_pkg::action::MyAction;
using GoalHandle = rclcpp_action::ClientGoalHandle<MyAction>;

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("cre_client");
  auto action_client = rclcpp_action::create_client<MyAction>(node, "my_action");
  RCLCPP_INFO(node->get_logger(), "Waiting for action server to start...");
  if (!action_client->wait_for_action_server(std::chrono::seconds(30))) 
  {
    RCLCPP_ERROR(node->get_logger(), "Action server not available!");
    rclcpp::shutdown();
    return 1;
  }

  MyAction::Goal goal;
  goal.object_name = "world";
  RCLCPP_INFO(node->get_logger(), "Sending goal...");

  auto send_goal_options = rclcpp_action::Client<MyAction>::SendGoalOptions();
  auto goal_handle_future = action_client->async_send_goal(goal, send_goal_options);

  if (rclcpp::spin_until_future_complete(node, goal_handle_future, std::chrono::seconds(30)) !=
      rclcpp::FutureReturnCode::SUCCESS)
  {
    RCLCPP_INFO(node->get_logger(), "Action did not finish before timeout.");
    rclcpp::shutdown();
    return 1;
  }

  auto goal_handle = goal_handle_future.get();
  if (!goal_handle) 
  {
    RCLCPP_ERROR(node->get_logger(), "Goal rejected by server");
    rclcpp::shutdown();
    return 1;
  }

  auto result_future = action_client->async_get_result(goal_handle);
  if (rclcpp::spin_until_future_complete(node, result_future, std::chrono::seconds(30)) !=
      rclcpp::FutureReturnCode::SUCCESS)
  {
    RCLCPP_INFO(node->get_logger(), "Action did not finish before timeout.");
    rclcpp::shutdown();
    return 1;
  }

  auto result = result_future.get();
  switch (result.code) 
  {
    case rclcpp_action::ResultCode::SUCCEEDED:
      RCLCPP_INFO(node->get_logger(), "Action finished: SUCCEEDED");
      break;
    case rclcpp_action::ResultCode::CANCELED:
      RCLCPP_INFO(node->get_logger(), "Action finished: CANCELED");
      break;
    default:
      RCLCPP_INFO(node->get_logger(), "Action finished: FAILED");
      break;
  }

  rclcpp::shutdown();
  return 0;
}
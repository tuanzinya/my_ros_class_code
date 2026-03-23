#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "my_class_pkg/action/my_action.hpp"

using namespace std::placeholders;
using MyAction = my_class_pkg::action::MyAction;
using GoalHandle = rclcpp_action::ServerGoalHandle<MyAction>;

class MyActionServer : public rclcpp::Node
{
public:
    explicit MyActionServer(const rclcpp::NodeOptions & options = rclcpp::NodeOptions())
    : Node("my_action_server", options)
    {
        this->action_server_ = rclcpp_action::create_server<MyAction>(
            this,
            "my_action", 
            std::bind(&MyActionServer::handle_goal, this, _1, _2),
            std::bind(&MyActionServer::handle_cancel, this, _1),
            std::bind(&MyActionServer::handle_accepted, this, _1)
        );

        RCLCPP_INFO(this->get_logger(), "Action 服务端已启动，等待目标...");
    }

private:
    rclcpp_action::Server<MyAction>::SharedPtr action_server_;

    rclcpp_action::GoalResponse handle_goal(
        const rclcpp_action::GoalUUID &,
        std::shared_ptr<const MyAction::Goal> goal)
    {
        RCLCPP_INFO(this->get_logger(), "%s Preempted!", goal->object_name.c_str());
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }

    rclcpp_action::CancelResponse handle_cancel(const std::shared_ptr<GoalHandle> goal_handle)
    {
        RCLCPP_INFO(this->get_logger(), "收到取消请求");
        return rclcpp_action::CancelResponse::ACCEPT;
    }

    void handle_accepted(const std::shared_ptr<GoalHandle> goal_handle)
    {
        std::thread{std::bind(&MyActionServer::execute, this, _1), goal_handle}.detach();
    }

    void execute(const std::shared_ptr<GoalHandle> goal_handle)
    {
        auto goal = goal_handle->get_goal();
        auto feedback = std::make_shared<MyAction::Feedback>();
        auto result = std::make_shared<MyAction::Result>();

        rclcpp::Rate rate(1);
        bool success = true;

        for (int i = 1; i <= 10; i++) 
        {
            if (goal_handle->is_canceling() || !rclcpp::ok()) 
            {
                RCLCPP_INFO(this->get_logger(), "动作被取消");
                goal_handle->canceled(result);
                success = false;
                return;
            }

            feedback->progress = i * 10.0f;
            RCLCPP_INFO(this->get_logger(), "Executing, progress: %.1f %%", feedback->progress);
            goal_handle->publish_feedback(feedback);

            rate.sleep();
        }

        if (success) 
        {
            result->success = true;
            RCLCPP_INFO(this->get_logger(), "Succeeded!");
            goal_handle->succeed(result);
        }
    }
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<MyActionServer>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
#include "rclcpp/rclcpp.hpp"
#include "my_class_pkg/srv/my_service_msg.hpp"

using MyServiceMsg = my_class_pkg::srv::MyServiceMsg;

class ServiceClient : public rclcpp::Node
{
public:
    ServiceClient() : Node("my_service_client")
    {
        client_ = this->create_client<MyServiceMsg>("my_service");
        while (!client_->wait_for_service(std::chrono::seconds(1))) {
            RCLCPP_INFO(this->get_logger(), "Waiting for server...");
        }
    }

    void send_request(int64_t input)
    {
        auto req = std::make_shared<MyServiceMsg::Request>();
        req->input = input;
        auto result = client_->async_send_request(req);
        if (rclcpp::spin_until_future_complete(this->get_node_base_interface(), result) ==
            rclcpp::FutureReturnCode::SUCCESS)
        {
            RCLCPP_INFO(this->get_logger(), "Service response: %ld", result.get()->output);
        }
    }

private:
    rclcpp::Client<MyServiceMsg>::SharedPtr client_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<ServiceClient>();
    node->send_request(42);
    rclcpp::shutdown();
    return 0;
}
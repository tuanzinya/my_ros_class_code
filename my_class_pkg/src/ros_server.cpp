#include "rclcpp/rclcpp.hpp"     
#include "my_class_pkg/srv/my_service_msg.hpp"

class ServiceServer : public rclcpp::Node
{
public:
    using MyServiceMsg = my_class_pkg::srv::MyServiceMsg;


    ServiceServer() : Node("my_service_server")
    {
        server_ = this->create_service<MyServiceMsg>(
            "my_service",
            std::bind(&ServiceServer::callback, this,
                std::placeholders::_1, std::placeholders::_2));
    }

private:
    bool callback(
        const MyServiceMsg::Request::SharedPtr req,
        MyServiceMsg::Response::SharedPtr res)
    {
        res->output = req->input * 2;
        RCLCPP_INFO(this->get_logger(), "Request:input = %ld, output = %ld", req->input, res->output);
        return true;
    }

    rclcpp::Service<MyServiceMsg>::SharedPtr server_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ServiceServer>());
    rclcpp::shutdown();
    return 0;
}

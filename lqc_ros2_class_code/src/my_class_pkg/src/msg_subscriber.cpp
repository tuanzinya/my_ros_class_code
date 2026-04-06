#include <memory>
#include "rclcpp/rclcpp.hpp"               
#include "my_class_pkg/msg/my_message.hpp"
using std::placeholders::_1;

class SubscriberNode : public rclcpp::Node
{
    public:
        SubscriberNode(): Node("my_message_subscriber") 
        {
            subscription_ = this->create_subscription<my_class_pkg::msg::MyMessage>(       
                "my_msg_topic", 10, std::bind(&SubscriberNode::topic_callback, this, _1));
        }

    private:
        void topic_callback(const my_class_pkg::msg::MyMessage::SharedPtr msg) const               
        {
            RCLCPP_INFO(this->get_logger(), "Received: key=%d, value='%s'", msg->key, msg->value.c_str());  
        }
        
        rclcpp::Subscription<my_class_pkg::msg::MyMessage>::SharedPtr subscription_;       
};

int main(int argc, char * argv[])                         
{
    rclcpp::init(argc, argv);                 
    rclcpp::spin(std::make_shared<SubscriberNode>());     
    rclcpp::shutdown();                                  
    
    return 0;
}
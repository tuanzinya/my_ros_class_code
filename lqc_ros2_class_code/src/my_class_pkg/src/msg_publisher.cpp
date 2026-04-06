#include <chrono>
#include <memory>
#include <string>
#include "rclcpp/rclcpp.hpp"          
#include "my_class_pkg/msg/my_message.hpp"

using namespace std::chrono_literals;
class PublisherNode : public rclcpp::Node
{
    public:
        PublisherNode()
        : Node("my_message_publisher")
        {
            publisher_ = this->create_publisher<my_class_pkg::msg::MyMessage>("my_msg_topic", 10); 
            timer_ = this->create_wall_timer(
                500ms, std::bind(&PublisherNode::timer_callback, this));            
        }

    private:
        // 创建定时器周期执行的回调函数
        void timer_callback()                                                       
        {
          auto msg = my_class_pkg::msg::MyMessage();  
          msg.key = key_;
          msg.value = value_ + std::to_string(msg.key);
          key_++;                                          
          RCLCPP_INFO(this->get_logger(), "Publishing: key=%d, value='%s'", msg.key, msg.value.c_str());
          publisher_->publish(msg);                                            
        }
        
        rclcpp::TimerBase::SharedPtr timer_;                            
        rclcpp::Publisher<my_class_pkg::msg::MyMessage>::SharedPtr publisher_;  
        int key_{0};
        std::string value_{"Hello from C++ Publisher, key = "};
};


int main(int argc, char * argv[])                      
{
    rclcpp::init(argc, argv);                       
    rclcpp::spin(std::make_shared<PublisherNode>());   
    rclcpp::shutdown();                                

    return 0;
}
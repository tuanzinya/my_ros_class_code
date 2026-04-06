#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include "rclcpp/rclcpp.hpp"          
#include "std_msgs/msg/string.hpp"    

using namespace std::chrono_literals;
class PublisherNode : public rclcpp::Node
{
    public:
        PublisherNode()
        : Node("my_publisher")
        {
            publisher_ = this->create_publisher<std_msgs::msg::String>("my_topic", 10); 
            timer_ = this->create_wall_timer(
                500ms, std::bind(&PublisherNode::timer_callback, this));            
        }

    private:
        // 创建定时器周期执行的回调函数
        void timer_callback()                                                       
        {
          auto msg = std_msgs::msg::String();                                    
          msg.data = "Hello World";                                                
          RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", msg.data.c_str());  
          publisher_->publish(msg);                                                
        }
        
        rclcpp::TimerBase::SharedPtr timer_;                            
        rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;  
};


int main(int argc, char * argv[])                      
{
    rclcpp::init(argc, argv);                       
    rclcpp::spin(std::make_shared<PublisherNode>());   
    rclcpp::shutdown();                                

    return 0;
}
# 实验二：话题、服务、动作消息实验
### 话题节点：
ros2 launch my_class_pkg bringup_topic.launch.py

ros2 run my_class_pkg my_message_publisher 

ros2 run my_class_pkg my_message_subscriber
### 服务节点：
ros2 run my_class_pkg my_service_server

ros2 run my_class_pkg my_service_client
### 动作节点
ros2 run my_class_pkg my_action_server

ros2 run my_class_pkg my_action_client
# 实验三：ros2参数的设置与应用、ros日志工具
### 参数
##### Python实现
ros2 run my_param_pkg dynamic_param.py 
##### C++实现
ros2 run my_param_pkg ros_param
##### Python实现动态控制小乌龟速度(double)
ros2 run my_param_pkg turtle_dynamic_speed.py 
### 日志工具
##### Python实现
ros2 run my_tools_pkg ros_log.py
##### C++实现
ros2 run my_tools_pkg ros_log

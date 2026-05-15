# 实验一：底盘驱动与里程计

## 轮式里程计

rosrun move_forward move_forward_node

## 碰撞传感器

rosrun sensor_pkg bump_avoid_node

## tof传感器

rosrun sensor_pkg tof_avoid_node

## imu实现自旋180°

rosrun sensor_pkg imu_spin_node

## odom实现正方形轨迹

rosrun odom_forward odom_forward_node

# 实验四：仿真功能实现

## Gazobo仿真

#### 模型文件在xacro目录下，chassis实现底盘，wheels实现轮胎，main是调用其他xacro文件

roslaunch lqc_robot_description gazebo.launch

## rviz仿真

roslaunch lqc_robot_description display.launch

# 实验五

## 一、深度相机驱动实验

rosrun visual_indetity get_ros_image.py

## 二、基于颜色识别的自主巡线实验

rosrun visual_indetity follow_line.py <br>
rostopic pub -1 /enable_move std_msgs/Int16 "data: 1"

## 三、基于手势识别的机器人控制实验

rosrun visual_indetity gesture_movement.py

## 四、视觉跟踪实验

rosrun visual_indetity apriltag_follow.py

## 五、视觉抓取实验

roslaunch upros_arm recognize_apriltag.launch <br>
rosrun visual_indetity tag_grab_node

# 实验六

## 一、订阅IMU传感器信息

rosrun chapt06_pkg ros_imu_node

## 二、基于IMU的自旋控制

rosrun chapt06_pkg ros_imu_rotate_node

## 三、激光雷达信息ROS获取

rosrun chapt06_pkg ros_scan_node

## 四、激光雷达避障

rosrun chapt06_pkg ros_avoid_node

## 五、机器人发送导航点

rosrun chapt06_pkg movebase_client_node

# 实验七

## 一、大模型调用

roslaunch upros_chat speech_to_word.launch <br>
rosrun chapt07_pkg llm_chat.py  

## 二、基于语音命令的抓取实验

roslaunch upros_bringup bringup_w2a.launch <br>
roslaunch upros_chat speech_to_word.launch <br>
roslaunch upros_arm recognize_apriltag.launch <br>
rosrun chapt07_pkg voice_control.py <br>
rosrun chapt07_pkg tag_grab_node

# 期末考核

## 依次启动：

roslaunch chapt08_pkg task_all.launch <br>
roslaunch chapt08_pkg fucking_run.launch

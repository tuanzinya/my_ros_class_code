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
### roslaunch lqc_robot_description gazebo.launch
#### 模型文件在xacro目录下，chassis实现底盘，wheels实现轮胎，main是主文件负责调用其他xacro
## rviz仿真
### roslaunch lqc_robot_description display.launch

# 实验一：底盘驱动与里程计
## 开环控制
rosrun move_forward move_forward_node
## 里程计闭环控制

## 碰撞、imu

# 实验四：仿真功能实现
## Gazobo仿真
### roslaunch lqc_robot_description gazebo.launch
#### 模型文件在xacro目录下，chassis实现底盘，wheels实现轮胎，main是主文件负责调用其他xacro
## rviz仿真
### roslaunch lqc_robot_description display.launch

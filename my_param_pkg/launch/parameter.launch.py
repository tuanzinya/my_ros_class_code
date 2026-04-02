from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            # 对应 ROS1 pkg
            package='my_param_pkg',
            
            # 对应 ROS1 type（可执行文件名）
            executable='ros_param',
            
            # 对应 ROS1 name
            name='parameter',
            
            # 对应 ROS1 output="screen"
            output='screen',
            
            # 对应 ROS1 <param name="my_param" value="ros!!!!"/>
            parameters=[
                {'my_param': 'ros!!!!'}
            ]
        )
    ])
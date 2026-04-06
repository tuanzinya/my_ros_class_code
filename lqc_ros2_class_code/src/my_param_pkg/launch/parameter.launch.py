from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='my_param_pkg',            
            executable='ros_param',            
            name='parameter',
            output='screen',
            parameters=[
                {'my_param': 'ros!!!!'}
            ]
        )
    ])
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package="my_class_pkg",
            executable="my_publisher",
            name="my_publisher",
            output="screen"
        ),

        Node(
            package="my_class_pkg",
            executable="my_subscriber",
            name="my_subscriber",
            output="screen"
        ),
    ])
#!/usr/bin/env python3
import rclpy
from rclpy.node import Node

def main(args=None):
    rclpy.init(args=args)

    node = Node("ros2_logging_py")
    node.get_logger().debug("This is a DEBUG message.")
    node.get_logger().info("This is an INFO message.")
    node.get_logger().warn("This is a WARNING message.")
    node.get_logger().error("This is an ERROR message.")
    node.get_logger().fatal("This is a FATAL message.")

    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from rcl_interfaces.msg import SetParametersResult, ParameterDescriptor, FloatingPointRange

class TurtleDynamicSpeed(Node):
    def __init__(self):
        super().__init__("turtle_dynamic_speed")

        self.turtle_speed = 0.0

        # ====================== 关键：创建浮点范围（让rqt显示滑动条）
        speed_range = FloatingPointRange()
        speed_range.from_value = -1.0  # 最小值
        speed_range.to_value = 1.0   # 最大值
        speed_range.step = 0.01         # 步长

        # ====================== 关键：给参数添加范围 + 描述
        param_desc = ParameterDescriptor(
            description="Turtle robot speed (-0.25 ~ 0.25)",
            floating_point_range=[speed_range]
        )

        # 声明带滑动条的动态参数
        self.declare_parameter("turtle_speed", 0.0, param_desc)

        # 注册回调
        self.add_on_set_parameters_callback(self.on_param_change)

        # 发布速度
        self.pub = self.create_publisher(Twist, "/turtle1/cmd_vel", 10)

        # 10Hz 定时发布
        self.timer = self.create_timer(0.1, self.pub_speed)

        self.get_logger().info("动态速度节点启动！滑动条范围：-1.0 ~ 1.0")

    # 参数回调
    def on_param_change(self, params):
        result = SetParametersResult()
        result.successful = True

        for param in params:
            if param.name == "turtle_speed":
                value = param.value

                # 双重保险：限制范围
                if value < -1.0 or value > 1.0:
                    result.successful = False
                    result.reason = "速度必须在 -1.0 ~ 1.0"
                    continue

                self.turtle_speed = value
                self.get_logger().info(f" 当前速度：{self.turtle_speed:.2f}")

        return result

    # 发布速度
    def pub_speed(self):
        msg = Twist()
        msg.linear.x = self.turtle_speed
        self.pub.publish(msg)

def main(args=None):
    rclpy.init(args=args)
    node = TurtleDynamicSpeed()
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == "__main__":
    main()
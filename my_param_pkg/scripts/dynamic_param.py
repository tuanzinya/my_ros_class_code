#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
# 导入速度消息（控制小乌龟）
from geometry_msgs.msg import Twist
# 导入参数范围类型（rqt滑动条核心）
from rcl_interfaces.msg import (
    SetParametersResult,
    ParameterDescriptor,
    IntegerRange,    # 整数滑动条
    FloatingPointRange # 浮点滑动条
)

class DynamicParamNode(Node):
    def __init__(self):
        super().__init__("dynamic_param_node")

        # ===================== 【新增】创建小乌龟速度发布者 =====================
        self.cmd_vel_pub = self.create_publisher(Twist, "/turtle1/cmd_vel", 10)

        # ===================== 定义滑动条范围（rqt可视化拖动） =====================
        # 整数参数滑动条：0~100
        int_range = IntegerRange()
        int_range.from_value = 0
        int_range.to_value = 100
        int_range.step = 1

        # 浮点参数滑动条：0~1
        double_range = FloatingPointRange()
        double_range.from_value = 0.0
        double_range.to_value = 1.0
        double_range.step = 0.01

        # ===================== 定义参数描述（绑定滑动条+说明） =====================
        desc_int = ParameterDescriptor(
            description="An Integer parameter (0~100)",
            integer_range=[int_range]  # 绑定整数滑动条
        )
        desc_double = ParameterDescriptor(
            description="A double parameter (0~1) 【控制小乌龟速度】",
            floating_point_range=[double_range]  # 绑定浮点滑动条
        )
        desc_str = ParameterDescriptor(description="A string parameter")
        desc_bool = ParameterDescriptor(description="A Boolean parameter")
        desc_size = ParameterDescriptor(description="Size enum: 0=Small,1=Medium,2=Large,3=ExtraLarge")

        # ===================== 声明动态参数（带默认值 + 约束） =====================
        self.declare_parameter("int_param", 50, desc_int)
        self.declare_parameter("double_param", 0.5, desc_double)
        self.declare_parameter("str_param", "Hello World", desc_str)
        self.declare_parameter("bool_param", True, desc_bool)
        self.declare_parameter("size", 1, desc_size)

        # ===================== 注册参数变化回调 =====================
        self.add_on_set_parameters_callback(self.param_callback)

        self.get_logger().info("ROS2 动态参数节点启动完成！")

    # ===================== 【新增】发布小乌龟速度 =====================
    def publish_turtle_speed(self, speed):
        twist = Twist()
        twist.linear.x = speed  # 用double_param控制线速度
        twist.angular.z = 0.0   # 不转向
        self.cmd_vel_pub.publish(twist)

    # ===================== 参数回调（动态修改时触发） =====================
    def param_callback(self, params):
        result = SetParametersResult()
        result.successful = True

        for param in params:
            name = param.name
            value = param.value

            # 范围限制（和你ROS1 cfg完全一样）
            if name == "int_param":
                if not (0 <= value <= 100):
                    result.successful = False
                    result.reason = "int_param 必须在 0~100"

            elif name == "double_param":
                if not (0 <= value <= 1):
                    result.successful = False
                    result.reason = "double_param 必须在 0~1"
                # 【核心】double_param变化 → 发布速度
                if result.successful:
                    self.publish_turtle_speed(value)

            elif name == "size":
                if not (0 <= value <= 3):
                    result.successful = False
                    result.reason = "size 必须 0~3"

            # 打印变化
            if result.successful:
                self.get_logger().info(f"参数更新：{name} = {value}")

        return result

def main(args=None):
    rclpy.init(args=args)
    node = DynamicParamNode()
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == "__main__":
    main()
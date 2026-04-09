#!/usr/bin/env python3
import rospy
import cv2
from sensor_msgs.msg import Image
from geometry_msgs.msg import Twist
from cv_bridge import CvBridge, CvBridgeError
import apriltag

class ImageSubscriberNode:
    def __init__(self):
        # 初始化ROS节点
        rospy.init_node('image_subscriber_node', anonymous=True)
        
        # 初始化AprilTag识别器
        self.tag_detector = apriltag.Detector(apriltag.DetectorOptions(families='tag36h11'))
        
        # 待跟踪的AprilTag ID
        self.follow_tag_id = 1
        
        # 初始化OpenCV和ROS图像转换桥梁
        self.bridge = CvBridge()
        
        # 订阅相机RGB图像
        self.image_sub = rospy.Subscriber('/camera/color/image_raw', Image, self.image_callback)
        
        # 发布处理后的图像
        self.image_pub = rospy.Publisher('/image_result', Image, queue_size=10)
        
        # 发布机器人速度控制指令
        self.vel_pub = rospy.Publisher('/cmd_vel', Twist, queue_size=10)
        
        rospy.loginfo("AprilTag跟踪节点已启动，跟踪ID: %d", self.follow_tag_id)

    def image_callback(self, msg):
        try:
            # 将ROS图像消息转换为OpenCV格式
            cv_image = self.bridge.imgmsg_to_cv2(msg, "bgr8")
            frame = cv_image.copy()
            # 转换为灰度图（AprilTag检测需要灰度图）
            gray_frame = cv2.cvtColor(cv_image, cv2.COLOR_BGR2GRAY)
            height, width = gray_frame.shape[:2]
            
            # 检测AprilTag
            tags = self.tag_detector.detect(gray_frame)
            
            # 初始化速度指令
            twist = Twist()
            
            # 遍历检测结果
            for tag in tags:
                if tag.tag_id == self.follow_tag_id:
                    # 获取标签角点和中心
                    top_left, top_right, bottom_right, bottom_left = tag.corners
                    center_x = int((top_left[0] + bottom_right[0]) / 2)
                    center_y = int((top_left[1] + bottom_right[1]) / 2)
                    
                    # 计算标签面积
                    tag_width = bottom_right[0] - top_left[0]
                    tag_height = bottom_right[1] - top_left[1]
                    area = int(tag_width * tag_height)
                    
                    # 在图像上绘制中心十字
                    cv2.line(frame, (center_x - 20, center_y), (center_x + 20, center_y), (0, 0, 255), 2)
                    cv2.line(frame, (center_x, center_y - 20), (center_x, center_y + 20), (0, 0, 255), 2)
                    
                    # 绘制标签边框
                    cv2.rectangle(frame, (int(top_left[0]), int(top_left[1])), 
                                  (int(bottom_right[0]), int(bottom_right[1])), (0, 255, 0), 2)
                    
                    # 水平方向控制（左右转向）
                    if center_x > width / 2 + 30:
                        twist.angular.z = -0.4  # 右转
                    elif center_x < width / 2 - 30:
                        twist.angular.z = 0.4   # 左转
                    else:
                        twist.angular.z = 0      # 居中
                    
                    # 前后方向控制（距离控制）
                    if area < 400:
                        twist.linear.x = 0.2     # 前进
                    elif area > 800:
                        twist.linear.x = -0.1    # 后退
                    else:
                        twist.linear.x = 0       # 保持距离
                    
                    # 发布速度指令
                    self.vel_pub.publish(twist)
            
            # 发布处理后的图像
            ros_image = self.bridge.cv2_to_imgmsg(frame, "bgr8")
            self.image_pub.publish(ros_image)

        except CvBridgeError as e:
            rospy.logerr(f"图像转换错误: {e}")
        except Exception as e:
            rospy.logerr(f"处理错误: {e}")

    def spin(self):
        rospy.spin()

if __name__ == '__main__':
    try:
        node = ImageSubscriberNode()
        node.spin()
    except rospy.ROSInterruptException:
        rospy.loginfo("节点已关闭")

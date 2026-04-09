#!/usr/bin/env python3
import rospy
import cv2
import mediapipe as mp
import math
from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError
from std_srvs.srv import Empty, EmptyRequest
# import sys
# sys.path.append("/home/bcsh/lqc_ros_class_ws/src/visual_indetity/scripts")

class UP_Gesture:
    def __init__(self): 
        self.draw = mp.solutions.drawing_utils
        self.hands = mp.solutions.hands.Hands(static_image_mode=False, max_num_hands=2,
        min_detection_confidence=0.75, min_tracking_confidence=0.75)

    # 检测人手函数，反馈 21 个点
    def findHand(self, img):  
        imgRGB = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        handlmsstyle = self.draw.DrawingSpec(color=(0, 0, 255), thickness=5)
        handconstyle = self.draw.DrawingSpec(color=(0, 255, 0), thickness=5)
        results = self.hands.process(imgRGB)
        if results.multi_hand_landmarks:
            for handLms in results.multi_hand_landmarks:
                self.draw.draw_landmarks(img,handLms,mp.solutions.hands.HAND_CONNECTIONS, handlmsstyle, handconstyle)
        return results.multi_hand_landmarks

    def detectNumber(self, hand_landmarks, img):
        h, w, _ = img.shape  
        hand_landmark = hand_landmarks[0].landmark

        thumb_tip_id = 4
        index_finger_tip_id = 8
        middle_finger_tip_id = 12
        ring_finger_tip_id = 16
        pinky_finger_tip_id = 20
        pinky_finger_mcp_id = 17
        wrist_id = 0

        thumb_tip_y = hand_landmark[thumb_tip_id].y * h
        index_tip_y = hand_landmark[index_finger_tip_id].y * h
        middle_tip_y = hand_landmark[middle_finger_tip_id].y * h
        ring_tip_y = hand_landmark[ring_finger_tip_id].y * h
        pinky_tip_y = hand_landmark[pinky_finger_tip_id].y * h
        pinky_mcp_y = hand_landmark[pinky_finger_mcp_id].y * h
        wrist_y = hand_landmark[wrist_id].y * h

        thumb_tip_x = hand_landmark[thumb_tip_id].x * w
        index_tip_x = hand_landmark[index_finger_tip_id].x * w
        middle_tip_x = hand_landmark[middle_finger_tip_id].x * w
        ring_tip_x = hand_landmark[ring_finger_tip_id].x * w
        pinky_tip_x = hand_landmark[pinky_finger_tip_id].x * w
        pinky_mcp_x = hand_landmark[pinky_finger_mcp_id].x * w
        wrist_x = hand_landmark[wrist_id].x * w

        dist_thumb2wrist = math.sqrt((thumb_tip_x - wrist_x)**2 + (thumb_tip_y - wrist_y)**2)
        dist_index2wrist = math.sqrt((index_tip_x - wrist_x) ** 2 + (index_tip_y - wrist_y) ** 2)
        dist_middle2wrist = math.sqrt((middle_tip_x - wrist_x) ** 2 + (middle_tip_y - wrist_y) ** 2)
        dist_ring2wrist = math.sqrt((ring_tip_x - wrist_x) ** 2 + (ring_tip_y - wrist_y) ** 2)
        dist_pinky2wrist = math.sqrt((pinky_tip_x - wrist_x) ** 2 + (pinky_tip_y - wrist_y) ** 2)
        dist_pinky_mcp2wrist = math.sqrt((thumb_tip_x - pinky_mcp_x)**2 + (thumb_tip_y - pinky_mcp_y)**2)

        dist_index2wrist_ratio = dist_index2wrist / dist_thumb2wrist
        dist_middle2wrist_ratio = dist_middle2wrist / dist_thumb2wrist
        dist_ring2wrist_ratio = dist_ring2wrist / dist_thumb2wrist
        dist_pinky2wrist_ratio = dist_pinky2wrist / dist_thumb2wrist
        dist_pinky_mcp2wrist_ratio = dist_pinky_mcp2wrist / dist_thumb2wrist

        if dist_index2wrist_ratio < 1.9 and dist_middle2wrist_ratio < 1.8 and dist_ring2wrist_ratio < 1.6 and dist_pinky2wrist_ratio < 1.4 and dist_pinky_mcp2wrist_ratio < 0.8:
            return 0
        elif 2.0 < dist_index2wrist_ratio and dist_middle2wrist_ratio < 1.8 and dist_ring2wrist_ratio < 1.6 and dist_pinky2wrist_ratio < 1.4 and dist_pinky_mcp2wrist_ratio < 0.8:
            return 1
        elif 2.0 < dist_index2wrist_ratio and 1.9 < dist_middle2wrist_ratio and dist_ring2wrist_ratio < 1.6 and dist_pinky2wrist_ratio < 1.4 and dist_pinky_mcp2wrist_ratio < 0.8:
            return 2
        elif 2.0 < dist_index2wrist_ratio and 1.9 < dist_middle2wrist_ratio and 1.75 < dist_ring2wrist_ratio and dist_pinky2wrist_ratio < 1.4 and dist_pinky_mcp2wrist_ratio < 0.8:
            return 3
        elif 2.0 < dist_index2wrist_ratio and 1.9 < dist_middle2wrist_ratio and 1.75 < dist_ring2wrist_ratio and 1.5 < dist_pinky2wrist_ratio and dist_pinky_mcp2wrist_ratio < 0.8:
            return 4
        elif dist_index2wrist_ratio > 0.5 and dist_middle2wrist_ratio > 0.5 and dist_ring2wrist_ratio > 0.5 and 0.9 < dist_pinky_mcp2wrist_ratio < 1.2:
            return 5
        elif dist_index2wrist_ratio < 0.5 and dist_middle2wrist_ratio < 0.5 and dist_ring2wrist_ratio < 0.5:
            return 6
        else:
            return -1

# 识别手势 0 闭合，5 张开
class ImageSubscriberNode:
    def __init__(self):
        rospy.init_node('gesture_control_node', anonymous=True)
        self.bridge = CvBridge()
        self.up_gesture = UP_Gesture()
        
        self.image_sub = rospy.Subscriber('/camera/color/image_raw', Image, self.image_callback)
        self.image_pub = rospy.Publisher('/image_result', Image, queue_size=10)
        self.last_result_number = -1  # 记录上一帧识别的手势

        try:
            # 等待服务可用
            rospy.wait_for_service('/upros_arm_control/grab_service', timeout=5.0)
            rospy.wait_for_service('/upros_arm_control/release_service', timeout=5.0)
            
            # 创建服务代理
            self.grab_proxy = rospy.ServiceProxy('/upros_arm_control/grab_service', Empty)
            self.release_proxy = rospy.ServiceProxy('/upros_arm_control/release_service', Empty)
            self.empty_req = EmptyRequest()
            rospy.loginfo("✅ 手势控制节点启动成功！")
            
        except rospy.ROSException as e:
            rospy.logerr(f"❌ 服务连接失败: {str(e)}")

    def image_callback(self, msg):
        try:
            cv_image = self.bridge.imgmsg_to_cv2(msg, "bgr8")
            result = cv_image.copy()
            
            # ✅ 这里必须是 findHand，和你类里的函数名一致
            hands_landmarks = self.up_gesture.findHand(result)  
            
            if hands_landmarks:
                resultNumber = self.up_gesture.detectNumber(hands_landmarks, result)
                
                if resultNumber >= 0:
                    # 显示识别数字
                    cv2.putText(result, str(resultNumber), (150, 150), 
                                cv2.FONT_HERSHEY_SIMPLEX, 3, (255, 0, 255), 5, cv2.LINE_AA)
                    
                    # 握拳 = 闭合
                    if resultNumber == 0:
                        if self.last_result_number != 0:
                            rospy.loginfo("🔴 夹爪闭合")
                            self.grab_proxy.call(self.empty_req)
                    
                    # 张开 = 打开
                    elif resultNumber == 5:
                        if self.last_result_number != 5:
                            rospy.loginfo("🟢 夹爪打开")
                            self.release_proxy.call(self.empty_req)
                    
                    self.last_result_number = resultNumber
                
                else:
                    cv2.putText(result, "NO NUMBER", (150, 150), 
                                cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
            
            # 发布图像
            ros_image = self.bridge.cv2_to_imgmsg(result, "bgr8")
            self.image_pub.publish(ros_image)
            
        except CvBridgeError as e:
            rospy.logerr(f"图像转换错误: {e}")
        except Exception as e:
            rospy.logerr(f"运行错误: {e}")

if __name__ == '__main__':
    try:
        node = ImageSubscriberNode()
        rospy.spin()
    except rospy.ROSInterruptException:
        pass

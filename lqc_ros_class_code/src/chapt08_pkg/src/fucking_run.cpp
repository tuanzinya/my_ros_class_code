#include <ros/ros.h>
#include <tf2_ros/transform_listener.h>
#include "chapt08_pkg/task_utils.h"

using task_utils::MoveBaseClient;

int main(int argc, char **argv)
{
    ros::init(argc, argv, "send_goal_node");
    ros::NodeHandle nh;

    ros::AsyncSpinner spinner(4);
    spinner.start();

    // ---- 可移植组件 ----
    task_utils::TagDetector      tag_detector("/tag_detections");
    task_utils::VoiceBroadcaster voice("/talk");
    tf2_ros::Buffer              tfBuffer;
    tf2_ros::TransformListener   tfListener(tfBuffer);
    MoveBaseClient               ac("move_base", true);

    ROS_INFO("Waiting for move_base action server...");
    ac.waitForServer();
    ROS_INFO("Connected to move_base server.");
    voice.speak("原神启动");

    // 运行时设定最大速度
    task_utils::setMaxVelocity(0.5, 0.5, 3.0);

    const int TARGET_TAG = 1;

    // ===== 目标点 1: 抓取 + 识别 =====
    task_utils::setYawTolerance(0.1);
    if (task_utils::navigateTo(ac, 2.4762, 1.7726, 1.3497, 1, 60.0, 2, &tfBuffer))
    {
        if (tag_detector.waitForTag(TARGET_TAG, 2.0))
        {
            ROS_INFO("已在目标点1处识别到物体 (Tag ID=%d)", TARGET_TAG);
            voice.speak("已找到目标");
        }
        else
        {
            ROS_WARN("fucking error — detected: %s", tag_detector.detectedIds().c_str());
            voice.speak("未找到目标");
        }
    }

    task_utils::setMaxVelocity(1.5, 1.5, 4.5);

    // ===== 途经点: Goal 1 → Goal 2, 不减速不重试 =====
    task_utils::setYawTolerance(3.14);
    task_utils::navigateThrough(ac, 0.443, 0.332, 99, tfBuffer);

    // ===== 途经点: Goal 1 → Goal 2, 不减速不重试 =====
    task_utils::setYawTolerance(3.14);
    task_utils::navigateThrough(ac, 0.25, 4.59, 99, tfBuffer);

    // ===== 目标点 2: 抓取 + 识别 =====
    task_utils::setYawTolerance(0.1);
    if (task_utils::navigateTo(ac, 2.4661, 3.4971, -1.5452, 2, 60.0, 2, &tfBuffer))
    {
        if (tag_detector.waitForTag(TARGET_TAG, 2.0))
        {
            ROS_INFO("已在目标点2处识别到物体 (Tag ID=%d)", TARGET_TAG);
            voice.speak("已找到目标");
        }
        else
        {
            ROS_WARN("fucking error — detected: %s", tag_detector.detectedIds().c_str());
            voice.speak("未找到目标");
        }
    }

    // ===== 途经点:Goal 1 -> 终点 经过即走 =====
    task_utils::setYawTolerance(3.14);
    task_utils::navigateThrough(ac, 1.12, 3.20, 99, tfBuffer);

    // ===== 终点 4: 精确到位，任务完成 =====
    task_utils::setYawTolerance(3.14);
    task_utils::navigateTo(ac, -0.768, 2.49, -1.5452, 3, 60.0, 2, &tfBuffer);

    task_utils::setYawTolerance(0.1);
    ROS_INFO("Mission complete.");
    ros::waitForShutdown();
    return 0;
}

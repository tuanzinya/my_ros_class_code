#ifndef CHAPT08_PKG_TASK_UTILS_H
#define CHAPT08_PKG_TASK_UTILS_H

#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_ros/transform_listener.h>
#include <std_msgs/String.h>
#include <apriltag_ros/AprilTagDetectionArray.h>
#include <dynamic_reconfigure/Reconfigure.h>
#include <vector>
#include <string>
#include <cmath>

namespace task_utils {

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

static const char* TEB_SRV = "/move_base/TebLocalPlannerROS/set_parameters";
static const double SPEED_RETRY_FACTOR = 0.7;  // 重试降速因子

// 速度状态（供重试机制恢复用）
static double g_vx = 0.75, g_vx_back = 0.75, g_vtheta = 4.5;

// ============================ 动态参数（dynamic_reconfigure） ============================

/** 设置 TEB yaw 到达容差（0.1=精确，3.14=忽略朝向） */
inline void setYawTolerance(double tolerance)
{
    dynamic_reconfigure::ReconfigureRequest req;
    dynamic_reconfigure::ReconfigureResponse resp;
    dynamic_reconfigure::DoubleParameter d;
    d.name = "yaw_goal_tolerance"; d.value = tolerance;
    req.config.doubles.push_back(d);

    ros::service::call(TEB_SRV, req, resp);
    ROS_INFO("[task_utils] yaw_goal_tolerance = %.2f rad", tolerance);
}

/** 设置 TEB 最大速度（通过 dynamic_reconfigure 立即生效） */
inline void setMaxVelocity(double vx = 1.00, double vx_back = 1.00, double vtheta = 4.5)
{
    g_vx = vx; g_vx_back = vx_back; g_vtheta = vtheta;

    dynamic_reconfigure::ReconfigureRequest req;
    dynamic_reconfigure::ReconfigureResponse resp;

    dynamic_reconfigure::DoubleParameter d;
    d.name = "max_vel_x"; d.value = vx;             req.config.doubles.push_back(d);
    d.name = "max_vel_x_backwards"; d.value = vx_back; req.config.doubles.push_back(d);
    d.name = "max_vel_theta"; d.value = vtheta;     req.config.doubles.push_back(d);

    ros::service::call(TEB_SRV, req, resp);
    ROS_INFO("[task_utils] max_vel: x=%.2f back=%.2f theta=%.2f", vx, vx_back, vtheta);
}

// ============================ AprilTag 检测器 ============================

class TagDetector
{
public:
    explicit TagDetector(const std::string &topic = "/tag_detections")
    {
        sub_ = ros::NodeHandle().subscribe(topic, 10, &TagDetector::callback, this);
    }

    bool waitForTag(int target_id, double timeout_s = 2.0)
    {
        ros::WallDuration(0.2).sleep();
        ros::spinOnce();

        ros::WallTime deadline = ros::WallTime::now() + ros::WallDuration(timeout_s);
        while (ros::ok() && ros::WallTime::now() < deadline)
        {
            for (int id : detected_tags_)
                if (id == target_id)
                    return true;
            ros::WallDuration(0.1).sleep();
            ros::spinOnce();
        }
        return false;
    }

    std::string detectedIds() const
    {
        if (detected_tags_.empty()) return "(none)";
        std::string out;
        for (size_t i = 0; i < detected_tags_.size(); ++i)
            out += std::to_string(detected_tags_[i]) + " ";
        out.pop_back();
        return out;
    }

private:
    ros::Subscriber sub_;
    std::vector<int> detected_tags_;

    void callback(const apriltag_ros::AprilTagDetectionArray::ConstPtr &msg)
    {
        detected_tags_.clear();
        for (auto &d : msg->detections)
            for (int id : d.id)
                detected_tags_.push_back(id);
    }
};

// ============================ 语音播报 ============================

class VoiceBroadcaster
{
public:
    explicit VoiceBroadcaster(const std::string &topic = "/talk")
    {
        pub_ = ros::NodeHandle().advertise<std_msgs::String>(topic, 10);
    }

    bool speak(const std::string &text)
    {
        if (pub_.getNumSubscribers() == 0)
        {
            ROS_WARN("[Voice] No subscriber on %s, skipped.", pub_.getTopic().c_str());
            return false;
        }
        std_msgs::String msg;
        msg.data = text;
        pub_.publish(msg);
        ROS_INFO("[Voice] Published: \"%s\" — %d subscriber(s)",
                 text.c_str(), pub_.getNumSubscribers());
        ros::WallDuration(1.5).sleep();
        return true;
    }

private:
    ros::Publisher pub_;
};

// ============================ 导航 ============================

inline bool navigateTo(MoveBaseClient &ac, double x, double y, double yaw,
                       int goal_id, double timeout_s = 60.0, int max_retries = 2,
                       tf2_ros::Buffer *tfBuf = nullptr)
{
    double orig_vx = g_vx, orig_vx_back = g_vx_back, orig_vtheta = g_vtheta;
    double cur_vx = orig_vx, cur_vx_back = orig_vx_back, cur_vtheta = orig_vtheta;

    for (int attempt = 0; attempt <= max_retries; ++attempt)
    {
        if (attempt > 0)
        {
            cur_vx     *= SPEED_RETRY_FACTOR;
            cur_vx_back *= SPEED_RETRY_FACTOR;
            cur_vtheta  *= SPEED_RETRY_FACTOR;
            setMaxVelocity(cur_vx, cur_vx_back, cur_vtheta);
            ROS_WARN("[navigateTo] Retry %d/%d speed ↓: x=%.2f back=%.2f theta=%.2f",
                     attempt, max_retries, cur_vx, cur_vx_back, cur_vtheta);
        }

        move_base_msgs::MoveBaseGoal goal;
        tf2::Quaternion q;
        q.setRPY(0, 0, yaw);

        goal.target_pose.pose.position.x = x;
        goal.target_pose.pose.position.y = y;
        goal.target_pose.pose.position.z = 0.0;
        goal.target_pose.pose.orientation.z = q.z();
        goal.target_pose.pose.orientation.w = q.w();
        goal.target_pose.header.frame_id = "map";
        goal.target_pose.header.stamp = ros::Time::now();

        ROS_INFO("[navigateTo] Goal %d: (%.4f, %.4f) yaw=%.4f (attempt %d)",
                 goal_id, x, y, yaw, attempt);
        ac.sendGoal(goal);

        // 轮询等待，同时监控距离
        bool near_goal = false;
        ros::Time start = ros::Time::now();

        while (ros::ok())
        {
            // 检查是否已完成
            if (ac.getState().isDone())
                break;

            // 接近目标 0.3m 内降速（仅触发一次）
            if (tfBuf && !near_goal)
            {
                try
                {
                    auto t = tfBuf->lookupTransform("map", "base_link",
                                ros::Time(0), ros::Duration(0.3));
                    double dx = t.transform.translation.x - x;
                    double dy = t.transform.translation.y - y;
                    double dist = std::sqrt(dx * dx + dy * dy);

                    if (dist < 0.3)
                    {
                        near_goal = true;
                        double slow_vx     = cur_vx * 0.5;
                        double slow_vx_back = cur_vx_back * 0.5;
                        double slow_vtheta  = cur_vtheta * 0.5;
                        setMaxVelocity(slow_vx, slow_vx_back, slow_vtheta);
                        ROS_INFO("[navigateTo] Goal %d: < 0.3 m, speed ↓ 0.5x (%.2f m/s)",
                                 goal_id, slow_vx);
                    }
                }
                catch (const tf2::TransformException &) {}
            }

            // 超时
            if ((ros::Time::now() - start).toSec() > timeout_s)
                break;

            ros::WallDuration(0.05).sleep();
        }

        auto state = ac.getState();
        if (state == actionlib::SimpleClientGoalState::SUCCEEDED)
        {
            ROS_INFO("[navigateTo] Goal %d reached.", goal_id);
            setMaxVelocity(orig_vx, orig_vx_back, orig_vtheta);
            return true;
        }
        else if (state.isDone())
        {
            ROS_WARN("[navigateTo] Goal %d failed: %s", goal_id, state.getText().c_str());
        }
        else
        {
            ROS_WARN("[navigateTo] Goal %d timed out.", goal_id);
            ac.cancelGoal();
        }
    }

    setMaxVelocity(orig_vx, orig_vx_back, orig_vtheta);
    ROS_ERROR("[navigateTo] Goal %d failed after %d retries, speed restored.", goal_id, max_retries);
    return false;
}

inline void navigateThrough(MoveBaseClient &ac, double x, double y, int goal_id,
                            tf2_ros::Buffer &tfBuf, double radius = 0.5,
                            double timeout_s = 30.0)
{
    move_base_msgs::MoveBaseGoal goal;
    tf2::Quaternion q;
    q.setRPY(0, 0, 0);

    goal.target_pose.pose.position.x = x;
    goal.target_pose.pose.position.y = y;
    goal.target_pose.pose.position.z = 0.0;
    goal.target_pose.pose.orientation.z = q.z();
    goal.target_pose.pose.orientation.w = q.w();
    goal.target_pose.header.frame_id = "map";
    goal.target_pose.header.stamp = ros::Time::now();

    ROS_INFO("[navigateThrough] Via %d: (%.4f, %.4f) radius=%.2f m",
             goal_id, x, y, radius);
    ac.sendGoal(goal);

    double distance = 1e9;
    ros::Time start = ros::Time::now();

    while (ros::ok())
    {
        try
        {
            auto t = tfBuf.lookupTransform("map", "base_link",
                         ros::Time(0), ros::Duration(0.5));
            double dx = t.transform.translation.x - x;
            double dy = t.transform.translation.y - y;
            distance = std::sqrt(dx * dx + dy * dy);
        }
        catch (const tf2::TransformException &) {}

        if (distance < radius)
        {
            ROS_INFO("[navigateThrough] Via %d passed (%.2f m).", goal_id, distance);
            break;
        }
        if ((ros::Time::now() - start).toSec() > timeout_s)
        {
            ROS_WARN("[navigateThrough] Via %d timeout (%.2f m).", goal_id, distance);
            break;
        }
        ros::WallDuration(0.1).sleep();
    }

    ac.cancelGoal();
    ros::WallDuration(0.3).sleep();
}

} // namespace task_utils
#endif // CHAPT08_PKG_TASK_UTILS_H

#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <tf2/LinearMath/Quaternion.h>
#include <geometry_msgs/Quaternion.h>
#include <iostream>
#include <cmath>

// Custom message for voice command (intent + target ID)
#include <upros_message/TagCommand.h>

using namespace std;
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

// Global pointer to move_base client for callback access
MoveBaseClient* ac_ptr;

/**
 * @brief Send navigation goal to move_base
 * @param goal_id 0 = home, 1 = goal1, 2 = goal2
 * @return true if goal reached successfully
 */
bool send_goal(int goal_id) {
    move_base_msgs::MoveBaseGoal goal;
    tf2::Quaternion quaternion;
    quaternion.setRPY(0, 0, 0); // Keep yaw angle at 0 (no rotation)

    // Set target coordinates based on goal ID
    switch(goal_id) {
        case 1: // Goal 1 position
            goal.target_pose.pose.position.x = 1.12;
            goal.target_pose.pose.position.y = 1.05;
            ROS_INFO("Navigating to Goal 1 (x:1.12, y:1.05)");
            break;
        case 2: // Goal 2 position
            goal.target_pose.pose.position.x = 2.41;
            goal.target_pose.pose.position.y = 1.96;
            ROS_INFO("Navigating to Goal 2 (x:2.41, y:1.96)");
            break;
        case 0: // Home / origin position
            goal.target_pose.pose.position.x = 0.0;
            goal.target_pose.pose.position.y = 0.0;
            goal.target_pose.pose.orientation.z = 0.0;
            goal.target_pose.pose.orientation.w = 1.0;
            ROS_INFO("Returning to Home (x:0.0, y:0.0)");
            break;
        default:
            ROS_WARN("Invalid goal ID: %d. Only 0(home)/1/2 supported", goal_id);
            return false;
    }

    // Apply orientation for non-home goals
    if(goal_id != 0) {
        goal.target_pose.pose.orientation.z = quaternion.z();
        goal.target_pose.pose.orientation.w = quaternion.w();
    }

    // Set frame and timestamp
    goal.target_pose.header.frame_id = "map";
    goal.target_pose.header.stamp = ros::Time::now();

    // Send goal and wait for result
    ac_ptr->sendGoal(goal);
    ac_ptr->waitForResult();

    // Check navigation result
    if (ac_ptr->getState() == actionlib::SimpleClientGoalState::SUCCEEDED) {
        ROS_INFO("Successfully reached Goal %d!", goal_id);
        return true;
    } else {
        ROS_WARN("Failed to reach Goal %d!", goal_id);
        return false;
    }
}

/**
 * @brief Callback function for voice command topic
 * @param msg Received voice command (intent + target)
 */
void voice_cmd_callback(const upros_message::TagCommand::ConstPtr& msg) {
    ROS_INFO("Received voice command: intent=%s, target=%d", msg->intent.c_str(), msg->target);
    
    // Only handle "go_to" navigation commands
    if(msg->intent == "go_to") {
        int target_id = msg->target;
        send_goal(target_id);
    } else {
        ROS_WARN("Unsupported intent: %s. Only 'go_to' is supported", msg->intent.c_str());
    }
}

int main(int argc, char **argv) {
    // Initialize ROS node
    ros::init(argc, argv, "speech_control_movebase_node");
    ros::NodeHandle nh;

    // Create move_base action client
    MoveBaseClient ac("move_base", true);
    ac_ptr = &ac;

    // Wait for move_base action server
    ROS_INFO("Waiting for move_base server to start...");
    if(!ac.waitForServer(ros::Duration(10.0))) {
        ROS_ERROR("move_base server did not respond within 10 seconds!");
        return -1;
    }
    ROS_INFO("Connected to move_base server");

    // Subscribe to voice control command topic
    ros::Subscriber cmd_sub = nh.subscribe("/voice_control", 10, voice_cmd_callback);

    ROS_INFO("Voice control navigation node started. Waiting for commands...");
    ros::spin(); // Keep node running and listening

    return 0;
}

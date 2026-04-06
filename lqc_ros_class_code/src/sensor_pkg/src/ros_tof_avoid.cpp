#include "ros/ros.h"
#include "sensor_msgs/Range.h"
#include "geometry_msgs/Twist.h"

ros::Publisher vel_pub;
const float DISTANCE_THRESHOLD = 0.4;

void tofFrontCallback(const sensor_msgs::Range::ConstPtr& msg)
{
    geometry_msgs::Twist vel_msg;
    float front_distance = msg->range;

    if (front_distance < DISTANCE_THRESHOLD && front_distance >= msg->min_range)
    {
        vel_msg.linear.x = -0.1;
        vel_msg.angular.z = 0.0;
    }
    else
    {
        vel_msg.linear.x = 0.1;
        vel_msg.angular.z = 0.0;
    }

    vel_pub.publish(vel_msg);
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "tof_avoid_obstacle");
    ros::NodeHandle nh;
    vel_pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
    ros::Subscriber sub_tof_front = nh.subscribe<sensor_msgs::Range>("/us/tof2", 10, tofFrontCallback);
    ros::spin();

    return 0;
}
#include "ros/ros.h"
#include "std_msgs/Int16MultiArray.h"
#include "geometry_msgs/Twist.h"
#include <unistd.h>

ros::Publisher vel_pub;
const double FORWARD_SPEED = 0.2;    
const double BACK_SPEED = -2.0;      
const double BACK_DISTANCE = 2.5;    
const int BACK_TIME_MS = (BACK_DISTANCE / fabs(BACK_SPEED)) * 1000;
bool is_colliding = false;


void bumpCallback(const std_msgs::Int16MultiArray::ConstPtr& msg)
{
    if (msg->data.size() > 1 && msg->data[1] == 1)
    {
        if (is_colliding) 
            return;
        is_colliding = true;

        geometry_msgs::Twist vel_msg;
        vel_msg.linear.x = 0.0;
        vel_pub.publish(vel_msg);
        usleep(20000);
        vel_msg.linear.x = BACK_SPEED;
        vel_pub.publish(vel_msg);
        usleep(BACK_TIME_MS * 1000);
        is_colliding = false;
    }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "bump_avoid_cycle_node");
    ros::NodeHandle n;
    vel_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 5);
    ros::Subscriber sub = n.subscribe("/robot/bump_sensor", 20, bumpCallback);
    ros::Rate loop_rate(100);

    while (ros::ok())
    {
        if (!is_colliding)
        {
            geometry_msgs::Twist vel_msg;
            vel_msg.linear.x = FORWARD_SPEED;
            vel_pub.publish(vel_msg);
        }

        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}
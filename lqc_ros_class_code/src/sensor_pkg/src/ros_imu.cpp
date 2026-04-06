#include "ros/ros.h"
#include "sensor_msgs/Imu.h"
#include "geometry_msgs/Twist.h"
#include "tf/transform_datatypes.h"
#include <cmath>

ros::Publisher vel_pub;          
double init_yaw = NAN;           
double target_angle = M_PI;      
double angle_tolerance = 0.087;  
bool is_rotating = false;        
const double spin_angular_vel = 0.5;


double normalize_angle(double angle)
{
    while (angle > M_PI) 
        angle -= 2 * M_PI;
    while (angle < -M_PI) 
        angle += 2 * M_PI;
    return angle;
}

void imu_spin_callback(const sensor_msgs::Imu::ConstPtr& imu_msg)
{
    tf::Quaternion quat;
    tf::quaternionMsgToTF(imu_msg->orientation, quat);
    double roll, pitch, yaw;
    tf::Matrix3x3(quat).getRPY(roll, pitch, yaw);
    yaw = normalize_angle(yaw);

    if (std::isnan(init_yaw))
    {
        init_yaw = yaw;
        is_rotating = true; // 开始自旋
        return;
    }
    double delta_yaw = normalize_angle(yaw - init_yaw);
    geometry_msgs::Twist vel_cmd;
    if (is_rotating && fabs(delta_yaw) < (target_angle - angle_tolerance))
    {
        vel_cmd.angular.z = spin_angular_vel;
        vel_pub.publish(vel_cmd);
    }
    else if (is_rotating)
    {
        vel_cmd.angular.z = 0.0;
        vel_pub.publish(vel_cmd);
        is_rotating = false;
    }
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "imu_spin_node");
    ros::NodeHandle nh;
    vel_pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
    ros::Subscriber imu_sub = nh.subscribe<sensor_msgs::Imu>("/imu/data", 10, imu_spin_callback);
    ros::spin();
    return 0;
}
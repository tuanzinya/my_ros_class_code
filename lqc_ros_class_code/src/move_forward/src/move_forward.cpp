#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <iostream>

int main(int argc, char **argv)
{
    ros::init(argc, argv, "move_forward_node");
    ros::NodeHandle nh;

    ros::Publisher pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 10);

    ros::Rate loop_rate(10); // 10Hz，即每 100 毫秒一次
    geometry_msgs::Twist vel_msg;
    vel_msg.linear.x = 0.0;
    vel_msg.linear.y = 0;
    vel_msg.linear.z = 0;
    vel_msg.angular.x = 0;
    vel_msg.angular.y = 0;
    vel_msg.angular.z = 0;
    int count = 0;

    //1.
    for(int i = 0; i < 4; i++)
    {
	vel_msg.linear.x = 1.0;
	while (ros::ok() && count < 30)
	{ 
	pub.publish(vel_msg);
	ros::spinOnce();
	loop_rate.sleep();
	count++;
	}
	count = 0;
	vel_msg.linear.x = 0.0;
	pub.publish(vel_msg);
	std::cout << "move forward!\n";

	//2. 
	vel_msg.angular.z = 30.0;
	while (ros::ok() && count < 10)
	{ 
	pub.publish(vel_msg);
	ros::spinOnce();
	loop_rate.sleep();
	count++;
	}
	count = 0;
	vel_msg.angular.z = 0;
	pub.publish(vel_msg);
	std::cout << "turn!\n";
    }
    /*//3.
    vel_msg.linear.x = 1.0;
    while (ros::ok() && count < 30)
    { 
        pub.publish(vel_msg);
        ros::spinOnce();
        loop_rate.sleep();
        count++;
    }
    count = 0;
    vel_msg.linear.x = 0.0;
    pub.publish(vel_msg);
    std::cout << "move forward!\n";   
    */
	
    return 0;
}


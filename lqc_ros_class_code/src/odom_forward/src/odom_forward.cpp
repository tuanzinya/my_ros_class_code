#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <tf/transform_datatypes.h>
#include <cmath>

enum State
{
    FORWARD,
    TURN,
    FINISH
};

class MoveSquare
{
public:
    MoveSquare()
    {
        ros::NodeHandle nh;
        cmd_pub_ = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
        odom_sub_ = nh.subscribe("/odom", 10, &MoveSquare::odomCallback, this);

        linear_speed_  = 0.2;        
        angular_speed_ = 0.5;        
        side_length_   = 1.0;        

        state_ = FORWARD;
        side_count_ = 0;
        got_odom_ = false;
        start_x_ = start_y_ = 0.0;
        start_yaw_ = 0.0;
    }

    void spin()
    {
        ros::Rate rate(10); // 10 Hz
        while (ros::ok())
        {
            ros::spinOnce();
            if (!got_odom_)
            {
                rate.sleep();
                continue;
            }

            geometry_msgs::Twist cmd;
            switch (state_)
            {
            case FORWARD:
                if (moveForward(cmd))
                {
                    state_ = TURN;
                    start_yaw_ = current_yaw_;
                }
                break;

            case TURN:
                if (turn90(cmd))
                {
                    side_count_++;
                    if (side_count_ >= 4)
                    {
                        state_ = FINISH;
                    }
                    else
                    {
                        state_ = FORWARD;
                        start_x_ = current_x_;
                        start_y_ = current_y_;
                    }
                }
                break;

            case FINISH:
                cmd.linear.x = 0.0;
                cmd.angular.z = 0.0;
                cmd_pub_.publish(cmd);
                return;
            }

            cmd_pub_.publish(cmd);
            rate.sleep();
        }
    }

private:
    void odomCallback(const nav_msgs::Odometry::ConstPtr &msg)
    {
        current_x_ = msg->pose.pose.position.x;
        current_y_ = msg->pose.pose.position.y;

        tf::Quaternion q(
            msg->pose.pose.orientation.x,
            msg->pose.pose.orientation.y,
            msg->pose.pose.orientation.z,
            msg->pose.pose.orientation.w);
        tf::Matrix3x3 m(q);
        double roll, pitch, yaw;
        m.getRPY(roll, pitch, yaw);
        current_yaw_ = yaw;

        if (!got_odom_)
        {
            got_odom_ = true;
            start_x_ = current_x_;
            start_y_ = current_y_;
            start_yaw_ = current_yaw_;
        }
    }

    bool moveForward(geometry_msgs::Twist &cmd)
    {
        double dx = current_x_ - start_x_;
        double dy = current_y_ - start_y_;
        double dist = std::sqrt(dx * dx + dy * dy);

        if (dist >= side_length_)
        {
            cmd.linear.x = 0.0;
            return true; // 这一条边完成
        }
        else
        {
            cmd.linear.x = linear_speed_;
            cmd.angular.z = 0.0;
            return false;
        }
    }

    double normalizeAngle(double angle)
    {
        while (angle > M_PI)
            angle -= 2.0 * M_PI;
        while (angle < -M_PI)
            angle += 2.0 * M_PI;
        return angle;
    }

    bool turn90(geometry_msgs::Twist &cmd)
    {
        double target_yaw = start_yaw_ + M_PI / 2.0;
        target_yaw = normalizeAngle(target_yaw);

        double error = normalizeAngle(target_yaw - current_yaw_);
        double abs_error = std::fabs(error);

        double yaw_tolerance = 0.01;

        if (abs_error < yaw_tolerance)
        {
            cmd.angular.z = 0.0;
            return true;
        }
        else
        {
            cmd.angular.z = (error > 0 ? angular_speed_ : -angular_speed_);
            cmd.linear.x = 0.0;
            return false;
        }
    }

private:
    ros::Publisher cmd_pub_;
    ros::Subscriber odom_sub_;

    double linear_speed_;
    double angular_speed_;
    double side_length_;

    double current_x_, current_y_, current_yaw_;
    double start_x_, start_y_, start_yaw_;
    bool got_odom_;

    State state_;
    int side_count_;
};

int main(int argc, char **argv)
{
    ros::init(argc, argv, "move_square_odom_node");
    MoveSquare mover;
    mover.spin();
    return 0;
}

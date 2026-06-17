#include <chrono>
#include <functional>
#include <memory>
#include <cmath>

#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"
#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

class FigureEightController : public rclcpp::Node
{
public:
  FigureEightController()
  : Node("figure_eight_controller"),
    linear_speed_(this->declare_parameter<float>("linear_speed", 2.0)),
    angular_speed_(this->declare_parameter<float>("angular_speed", 1.0)),
    circle_start_time_(this->now()),
    circle_count_(0),
    is_initialized_(false),
    T_time_(this->declare_parameter<float>("T_time", 6.2831852)), 
    complete_state_ (3)
  {
    // 创建订阅者：获取乌龟实时位姿
    pose_subscriber_ = this->create_subscription<turtlesim::msg::Pose>(
      "/turtle1/pose", 10,
      std::bind(&FigureEightController::pose_callback, this, std::placeholders::_1));

    // 创建发布者：发送速度指令
    turle_controller = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);

    // 创建定时器：控制周期 100ms
    control_timer_ = this->create_wall_timer(
      100ms, std::bind(&FigureEightController::control_loop, this));
  }

private:
  // ============ 位姿回调 ============
  void pose_callback(const turtlesim::msg::Pose::SharedPtr msg)
  {
    current_x_ = msg->x;
    current_y_ = msg->y;
    current_theta_ = msg->theta;
    
    if (!is_initialized_) {
      // 记录初始位置作为切点
      tangent_x_ = current_x_;
      tangent_y_ = current_y_;
      is_initialized_ = true;
      RCLCPP_INFO(this->get_logger(), 
                  "初始化完成, 切点位置: (%.2f, %.2f)", 
                  tangent_x_, tangent_y_);
    }

    RCLCPP_INFO(this->get_logger(), 
                "当前位姿: (%.2f, %.2f, %.2f)", 
                current_x_, current_y_, current_theta_);
  }

  // 角度归一化到 [-π, π]
  double normalize_angle(double angle)
  {
    while (angle > M_PI) angle -= 2 * M_PI;
    while (angle < -M_PI) angle += 2 * M_PI;
    return angle;
  }

  // ============ 控制主循环 ============
  void control_loop()
  {
    if (!is_initialized_) {
      // 还没收到位姿数据，等待
      return;
    }

    geometry_msgs::msg::Twist cmd;
    
    // 线速度：恒定
    cmd.linear.x = linear_speed_;
    cmd.linear.y = 0.0;
    cmd.linear.z = 0.0;
    cmd.angular.x = 0.0;
    cmd.angular.y = 0.0;

    auto elapsed = (this->now() - circle_start_time_).seconds();
    if (elapsed > T_time_) 
    {
      if (complete_state_ == 3) 
      {
        // 航母掉头 用一个控制周期转向切点
        // 计算从切点到当前位置的角度
        double dx = tangent_x_ - current_x_;
        double dy = tangent_y_ - current_y_;
        double angle_to_center = std::atan2(dy, dx);
        // 乌龟当前朝向与切点-当前位置连线的夹角
        double heading_error = normalize_angle(angle_to_center - current_theta_);
        cmd.linear.x = 0;
        cmd.angular.z = heading_error * 10;
        complete_state_ = 1;
      }
      else if (complete_state_ == 1) 
      {
        double dx = current_x_ - tangent_x_;
        double dy = current_y_ - tangent_y_;
        double distance = std::sqrt(dx*dx + dy*dy);
        cmd.linear.x = distance * 10;
        cmd.angular.z = 0;
        complete_state_ = 2;
      }
      else {
        cmd.linear.x = 0;
        cmd.angular.z = - current_theta_ * 10;
        complete_state_ = 3;
        circle_start_time_ = this -> now();
        circle_count_ ++;
      }
    }
    else 
    {
      if (circle_count_ & 1) 
      {
        cmd.angular.z = -angular_speed_;
      }
      else
      {
        cmd.angular.z = angular_speed_;
      }
    }

    // 发布指令
    RCLCPP_INFO(this->get_logger(),
                "发送运动指令: linear.x=%.2f, angular.z=%.2f",
                cmd.linear.x,
                cmd.angular.z);
    turle_controller->publish(cmd);
  }

  // ============ 成员变量 ============
  // 控制参数
  double linear_speed_;
  double angular_speed_;
  
  // 状态变量
  double current_x_ = 0.0;
  double current_y_ = 0.0;
  double current_theta_ = 0.0;
  double tangent_x_ = 0.0;
  double tangent_y_ = 0.0;
  
  rclcpp::Time circle_start_time_;
  int circle_count_ = 0;
  bool is_initialized_ = false;
  double T_time_;
  int complete_state_;

  // ROS2 通信对象
  rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr pose_subscriber_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr turle_controller;
  rclcpp::TimerBase::SharedPtr control_timer_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<FigureEightController>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}

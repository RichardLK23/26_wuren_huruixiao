#include <chrono>
#include <memory>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "visualization_msgs/msg/marker.hpp"
#include "visualization_msgs/msg/marker_array.hpp"
#include "fsd_common_msgs/msg/map.hpp"

using namespace std::chrono_literals;

class MapConeVisualizer : public rclcpp::Node
{
public:
    MapConeVisualizer() : Node("map_cone_visualizer")
    {
        // 订阅地图消息
        map_sub_ = this->create_subscription<fsd_common_msgs::msg::Map>(
            "/estimation/slam/map", 10,
            std::bind(&MapConeVisualizer::mapCallback, this, std::placeholders::_1));
        
        // 创建Marker发布器
        marker_pub_ = this->create_publisher<visualization_msgs::msg::MarkerArray>(
            "visualization_marker_array", 10);
        
        RCLCPP_INFO(this->get_logger(), "Map Cone Visualizer started");
    }

private:
    void publishCones(visualization_msgs::msg::MarkerArray& marker_array, 
                      const std::vector<fsd_common_msgs::msg::Cone>& cones,
                      float r, float g, float b,
                      int& cone_id)
    {
        for (const auto& cone : cones) {
            visualization_msgs::msg::Marker marker;
            
            marker.header.frame_id = "map";
            marker.header.stamp = this->now();
            marker.ns = "map_cones";
            marker.id = cone_id++;
            marker.type = visualization_msgs::msg::Marker::CYLINDER;
            marker.action = visualization_msgs::msg::Marker::ADD;
            
            // 设置位置
            marker.pose.position.x = cone.position.x;
            marker.pose.position.y = cone.position.y;
            marker.pose.position.z = 0.15;  // 锥桶高度的一半，使其底部在地面上
            
            // 设置方向（锥桶直立）
            marker.pose.orientation.x = 0.0;
            marker.pose.orientation.y = 0.0;
            marker.pose.orientation.z = 0.0;
            marker.pose.orientation.w = 1.0;
            
            // 设置锥桶尺寸：底部直径0.3m，高度0.3m
            marker.scale.x = 0.3;
            marker.scale.y = 0.3;
            marker.scale.z = 0.3;
            
            // 设置颜色
            marker.color.r = r;
            marker.color.g = g;
            marker.color.b = b;
            marker.color.a = 1.0f;
            
            // 生命周期（永久显示）
            marker.lifetime = rclcpp::Duration::from_seconds(0.0);
            
            marker_array.markers.push_back(marker);
        }
    }

    void mapCallback(const fsd_common_msgs::msg::Map::SharedPtr msg)
    {
        visualization_msgs::msg::MarkerArray marker_array;
        
        // 清除之前的Marker
        visualization_msgs::msg::Marker delete_marker;
        delete_marker.header.frame_id = "map";
        delete_marker.header.stamp = this->now();
        delete_marker.ns = "map_cones";
        delete_marker.action = visualization_msgs::msg::Marker::DELETEALL;
        marker_array.markers.push_back(delete_marker);
        
        int cone_id = 0;
        int total_cones = 0;
        
        // 发布黄色锥桶
        total_cones += msg->cone_yellow.size();
        publishCones(marker_array, msg->cone_yellow, 1.0f, 1.0f, 0.0f, cone_id);  // 黄色
        
        // 发布蓝色锥桶
        total_cones += msg->cone_blue.size();
        publishCones(marker_array, msg->cone_blue, 0.0f, 0.0f, 1.0f, cone_id);    // 蓝色
        
        // 发布红色锥桶
        total_cones += msg->cone_red.size();
        publishCones(marker_array, msg->cone_red, 1.0f, 0.0f, 0.0f, cone_id);     // 红色
        
        // 发布未知颜色锥桶（灰色）
        total_cones += msg->cone_unknown.size();
        publishCones(marker_array, msg->cone_unknown, 0.5f, 0.5f, 0.5f, cone_id); // 灰色
        
        // 发布所有锥桶
        marker_pub_->publish(marker_array);
        
        RCLCPP_INFO(this->get_logger(), 
                    "Published %d cones (Yellow: %zu, Blue: %zu, Red: %zu, Unknown: %zu)",
                    total_cones,
                    msg->cone_yellow.size(),
                    msg->cone_blue.size(),
                    msg->cone_red.size(),
                    msg->cone_unknown.size());
    }
    
    rclcpp::Subscription<fsd_common_msgs::msg::Map>::SharedPtr map_sub_;
    rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr marker_pub_;
};

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MapConeVisualizer>());
    rclcpp::shutdown();
    return 0;
}
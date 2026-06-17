import os
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    return LaunchDescription([
        # 锥桶可视化节点
        Node(
            package='publish_maker_package',
            executable='map_cone_visualizer',
            name='map_cone_visualizer',
            output='screen'
        ),
        
        # 启动RViz
        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            output='screen'
        )
    ])
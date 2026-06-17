from launch import LaunchDescription
from launch.substitutions import PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    turtlesim_node = Node(
        package='turtlesim',  #功能包名
        executable='turtlesim_node', #可执行文件名
        name='turtlesim',     #节点名称、
        output='screen',      #输出位置
    )

    turtle_controller_config = PathJoinSubstitution(
        [FindPackageShare('ros2_homework_basic_package'), 'config', 'ros2_homework_basic.yaml']
    )

    turle_controller_node = Node(
        package='ros2_homework_basic_package',
        executable='turle_controller',
        name='turle_controller',
        output='screen',
        parameters=[turtle_controller_config]
    )

    return LaunchDescription([
        turtlesim_node,
        turle_controller_node,
    ])
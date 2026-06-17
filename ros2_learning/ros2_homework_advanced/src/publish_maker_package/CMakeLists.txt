cmake_minimum_required(VERSION 3.8)
project(publish_maker_package)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 寻找依赖包
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(fsd_common_msgs REQUIRED)

# 添加可执行文件
add_executable(map_cone_visualizer src/publish_maker.cpp)

target_include_directories(map_cone_visualizer PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(map_cone_visualizer PUBLIC c_std_99 cxx_std_17)

# 链接依赖库
ament_target_dependencies(map_cone_visualizer
  rclcpp
  visualization_msgs
  fsd_common_msgs
)

# 安装可执行文件
install(TARGETS
  map_cone_visualizer
  DESTINATION lib/${PROJECT_NAME}
)

# 安装launch文件
install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
FILE (GLOB Path_A2 "src/*.cpp")

add_library(A2 STATIC ${Path_A2})

target_include_directories(A2 PUBLIC include)

# 在定义 A2 库或可执行文件之后添加
target_compile_options(A2 PUBLIC -include cstddef)
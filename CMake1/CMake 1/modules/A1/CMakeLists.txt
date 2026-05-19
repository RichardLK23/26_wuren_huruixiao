FILE (GLOB Path_A1 "src/*.cpp")

add_library(A1 STATIC ${Path_A1})

target_include_directories(A1 PUBLIC include)


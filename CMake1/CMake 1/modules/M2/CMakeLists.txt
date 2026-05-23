FILE (GLOB Path_M2 "src/*.cpp")

add_library(M2 STATIC ${Path_M2})

target_include_directories(M2 PUBLIC include)

target_link_libraries(M2 PUBLIC A1)
target_link_libraries(M2 PUBLIC A2)
target_include_directories(M2 PUBLIC ${kalman_INCLUDE_DIRS})


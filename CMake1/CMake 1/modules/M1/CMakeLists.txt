FILE (GLOB Path_M1 "src/*.cpp")

add_library(M1 STATIC ${Path_M1})

target_include_directories(M1 PUBLIC include)
target_link_libraries(M1 PUBLIC A1)
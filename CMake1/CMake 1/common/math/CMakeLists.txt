find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

FILE (GLOB Path_Math "src/*.cpp")

add_library(math STATIC ${Path_Math})

target_include_directories(math PUBLIC include)

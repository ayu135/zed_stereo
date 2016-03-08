cmake_minimum_required(VERSION 2.8.12)


project(stereo_opencv)




find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})


add_executable(stereo_opencv stereo_opencv.cpp)
target_link_libraries(stereo_opencv ${OpenCV_LIBS})




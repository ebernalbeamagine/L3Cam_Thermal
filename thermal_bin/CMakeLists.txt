cmake_minimum_required(VERSION 3.0)

project(thermal_bin)

find_package(OpenCV REQUIRED)

add_executable(thermal_bin main.cpp)

target_link_libraries(thermal_bin ${OpenCV_LIBS})
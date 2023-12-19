cmake_minimum_required(VERSION 3.0)

project(udp_thermal_reader)

find_package(Threads REQUIRED)  

add_executable(udp_thermal_reader main.cpp)

target_link_libraries(udp_thermal_reader PRIVATE Threads::Threads)
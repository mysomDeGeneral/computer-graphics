cmake_minimum_required(VERSION 3.24)
project(pyramid)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 3.3 REQUIRED)

add_executable(pyramid app.cpp lib/glad.c)

target_include_directories(pyramid SYSTEM PRIVATE lib/include)

target_link_libraries(pyramid glfw)

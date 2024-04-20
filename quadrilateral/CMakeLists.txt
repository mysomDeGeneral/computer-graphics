cmake_minimum_required(VERSION 3.24)
project(quadrilateral)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 3.3 REQUIRED)

add_executable(quadrilateral app.cpp lib/glad.c)

target_include_directories(quadrilateral SYSTEM PRIVATE lib/include)

target_link_libraries(quadrilateral glfw)

cmake_minimum_required(VERSION 3.24)
project(dvdanimation)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 3.3 REQUIRED)

add_executable(dvdanimation app.cpp lib/glad.c)

target_include_directories(dvdanimation SYSTEM PRIVATE lib/include)

target_link_libraries(dvdanimation glfw)

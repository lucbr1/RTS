cmake_minimum_required(VERSION 3.20)

project(RTS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 COMPONENTS Graphics Window System CONFIG REQUIRED)

add_executable(RTS RTSproject.cpp)

target_link_libraries(RTS
    PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)
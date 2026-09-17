cmake_minimum_required(VERSION 3.20)

project(RTS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 COMPONENTS Graphics Window System CONFIG REQUIRED)

add_executable(RTS
    RTSproject.cpp
    Entite.cpp
    Entite.hpp
    Proie.cpp
    Proie.hpp
    Predateur.cpp
    Predateur.hpp
    Grille.cpp
    Grille.hpp
    fonctions.cpp
    fonctions.hpp
)

target_link_libraries(RTS
    PRIVATE 
    SFML::Graphics
    SFML::Window
    SFML::System
)

add_custom_command(TARGET RTS POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:RTS>/assets
)
cmake_minimum_required(VERSION 3.9)

project(orbits_port LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package (Eigen3 3.4 REQUIRED NO_MODULE)
find_package (SFML REQUIRED system window graphics network audio)

add_executable(orbits_port main.cpp
               Body.h Body.cpp
               tools.h tools.cpp)

target_link_libraries (orbits_port PRIVATE sfml-audio sfml-graphics sfml-network sfml-system Eigen3::Eigen)

install(TARGETS orbits_port
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

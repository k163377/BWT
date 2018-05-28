cmake_minimum_required(VERSION 3.10)
project(BWT)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall /O3")

add_executable(BWT main.cpp)
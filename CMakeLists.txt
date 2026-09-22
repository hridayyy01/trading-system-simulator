cmake_minimum_required(VERSION 3.16)
project(trading-system VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(trading-system src/main.cpp)

target_include_directories(trading-system PRIVATE include)

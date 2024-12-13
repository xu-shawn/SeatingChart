cmake_minimum_required(VERSION 3.13)
project(SeatingChart)

set(CMAKE_CXX_COMPILER "c++")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
SET(CMAKE_CXX_FLAGS "-Wall")
SET(CMAKE_CXX_FLAGS_DEBUG "-g3")
SET(CMAKE_CXX_FLAGS_RELEASE "-march=native -O3 -Ofast -fno-stack-protector -fno-math-errno -funroll-loops -fno-exceptions -flto -flto-partition=one")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCE_FILES src/main.cpp src/utils.cpp)

add_executable(SeatingChart ${SOURCE_FILES})

cmake_minimum_required(VERSION 3.10)
project(AlgoPractice VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(algo algo.cpp)

add_executable(duplicate FindTheDuplicate.cpp)
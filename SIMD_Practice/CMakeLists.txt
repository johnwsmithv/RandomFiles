cmake_minimum_required(VERSION 3.16)

# Project name and language
project(SIMD_Practice LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-fopenmp -Wno-deprecated-enum-enum-conversion")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb3")

# Enable warnings
add_compile_options(-Wall -Wextra -Wpedantic)

#####################################################################################

# Test Executable

add_library(sum SHARED Sum.cpp)
target_compile_options(sum PRIVATE
    # -ggdb3
    -mavx2
    -fopenmp
)
target_include_directories(sum PRIVATE 
    ../etc/xsimd-13.2.0/include
)

add_executable(u_test_sum test/u_test_sum.cpp)
target_include_directories(u_test_sum PRIVATE 
    ../etc/googletest-1.16.0/include
    ../etc/xsimd-13.2.0/include
    ../utils/
    ${CMAKE_SOURCE_DIR}
)
target_link_directories(u_test_sum PRIVATE ../etc/googletest-1.16.0/lib)
target_link_libraries(u_test_sum gtest sum)
target_compile_options(u_test_sum PRIVATE
    -mavx2
    -fopenmp
)
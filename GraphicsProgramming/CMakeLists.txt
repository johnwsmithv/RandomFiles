cmake_minimum_required(VERSION 3.16)

# We need the project to support both C and C++ since glad is a C program!
project(RenderScreen LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb3")

#####################################################################################

add_library(glad SHARED etc/glad/glad.c)

add_executable(RenderScreen RenderScreen.cpp)
target_include_directories(RenderScreen PRIVATE etc/)
target_link_libraries(RenderScreen PRIVATE glfw glad)
target_compile_options(RenderScreen PRIVATE 
    -Wall -Wextra -Wpedantic
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Debug>:-ggdb3>
)
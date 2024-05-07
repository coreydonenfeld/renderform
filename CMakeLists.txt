cmake_minimum_required(VERSION 3.24)

project(ComputerVision_MathTextual)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_BUILD_TYPE "Debug")

if (WIN32)
    add_definitions(-DMATHTEXTUAL_MSCPP)
elseif (LINUX)
    add_definitions(-DMATHTEXTUAL_LINUX)
elseif (APPLE)
    add_definitions(-DMATHTEXTUAL_APPLE)
endif()

add_subdirectory(MathTextual)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ComputerVision_MathTextual)

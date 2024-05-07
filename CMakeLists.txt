cmake_minimum_required(VERSION 3.24)

project(ComputerVision_MathTextual)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_BUILD_TYPE "Debug")

# Platform
if (WIN32)
    add_definitions(-DMATHTEXTUAL_MSCPP)
elseif (LINUX)
    add_definitions(-DMATHTEXTUAL_LINUX)
elseif (APPLE)
    add_definitions(-DMATHTEXTUAL_APPLE)
endif()

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Tesseract
set(CMAKE_LIBRARY_PATH "/opt/homebrew/Cellar/libarchive/3.7.2/lib")
find_package(PkgConfig REQUIRED)
pkg_search_module(TESSERACT REQUIRED tesseract)
pkg_search_module(LEPTONICA REQUIRED lept)
include_directories(${TESSERACT_INCLUDE_DIRS})
include_directories(${LEPTONICA_INCLUDE_DIRS})
link_directories(${TESSERACT_LIBRARY_DIRS})
link_directories(${LEPTONICA_LIBRARY_DIRS})

# MathTextual
add_subdirectory(MathTextual)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ComputerVision_MathTextual)

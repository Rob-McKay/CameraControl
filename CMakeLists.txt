cmake_minimum_required(VERSION 3.19.2)
project(lscamera VERSION 1.0)

configure_file(LSCameraConfig.h.in LSCameraConfig.h)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_SYSTEM_NAME MacOS)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_OSX_ARCHITECTURES x86_64)

set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})

find_package(Poco REQUIRED)

add_compile_options(-Wall -Wextra -Wpedantic)

add_subdirectory(camera_interface)

add_executable(lscamera "List Cameras/main.cpp")
target_link_libraries(lscamera PUBLIC Poco::Poco camera_interface)
target_include_directories(lscamera PUBLIC 
    "${PROJECT_BINARY_DIR}"
    )

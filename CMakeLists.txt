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

message(STATUS "Checking compiler ID '${CMAKE_CXX_COMPILER_ID}'")


if (${CMAKE_CXX_COMPILER_ID} MATCHES "^(AppleClang|Clang)$" )
message(STATUS "Adding extra checks to compile flags")
add_compile_options(-Wall -Wextra -Wpedantic -Werror=return-type -pedantic-errors -Wfor-loop-analysis -Wheader-hygiene -Widiomatic-parentheses -Wimplicit-fallthrough -Wmicrosoft)
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES "^(MSVC)$" )
add_compile_options(-W4)
else()
message(WARNING "No additional compile options for '${CMAKE_CXX_COMPILER_ID}'")
endif()

add_subdirectory(camera_interface)

add_executable(lscamera "src/lscamera.cpp")
target_link_libraries(lscamera PUBLIC Poco::Poco camera_interface)
target_include_directories(lscamera PUBLIC 
    "${PROJECT_BINARY_DIR}"
    ${Poco_INCLUDE_DIRS}
    )

add_executable(cpimage "src/cpimage.cpp")
target_link_libraries(cpimage PUBLIC Poco::Poco camera_interface)
target_include_directories(cpimage PUBLIC 
    "${PROJECT_BINARY_DIR}"
    ${Poco_INCLUDE_DIRS}
    )

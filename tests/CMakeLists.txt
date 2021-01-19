set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_OSX_ARCHITECTURES x86_64)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# The next two lines are required to find conan managed resources
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})

find_package(Poco REQUIRED)
find_package(GTest REQUIRED)

set(CanonEDSDK ${CMAKE_SOURCE_DIR}/CanonEDSDK/Machintosh/EDSDK)

message(STATUS "Building for ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}")
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "MacOS")
    set(extra_libraries "-framework CoreFoundation")
else()
    # Add any other platform specific libraries required for other platforms
endif()

add_compile_options(-Wall -Wextra -Wpedantic -Wshadow)
add_compile_options(-arch x86_64)

add_executable(library_tests init_tests.cpp)

target_link_libraries(library_tests
    PUBLIC ${extra_libraries}
    ${CONAN_LIBS}
    GTest::GTest 
    Poco::Poco
    )

target_include_directories(library_tests
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    ${CanonEDSDK}/Header
    ${CMAKE_MODULE_PATH}
    ${Poco_INCLUDE_DIRS}
    ${GTest_INCLUDE_DIRS}
    )

add_test(NAME library_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR} COMMAND library_tests)

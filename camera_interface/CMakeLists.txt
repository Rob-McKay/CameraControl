#TODO: Needs conditional bits adding for windows
add_library(camera_interface STATIC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_OSX_ARCHITECTURES x86_64)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# The next two lines are required to find conan managed resources
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})

find_package(Poco REQUIRED)

set(CanonEDSDK ${CMAKE_SOURCE_DIR}/CanonEDSDK/Machintosh/EDSDK)
find_library(canon_edsdk NAMES EDSDK PATHS "${CanonEDSDK}/Framework" REQUIRED NO_DEFAULT_PATH)
message(VERBOSE "EDSDK found in '${canon_edsdk}'")

add_compile_options(-Wall -Wextra -Wpedantic -Wshadow)
add_compile_options(-arch x86_64)

message(STATUS "Building for ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}")
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "MacOS")
    set(extra_libraries "-framework CoreFoundation")
else()
    # Add any other platform specific libraries required for other platforms
endif()

message(VERBOSE "Using the following extra libraries: ${extra_libraries}")

target_sources(camera_interface 
    PRIVATE camera_info_impl 
    camera_interface.cpp 
    camera_list_impl.cpp 
    camera_ref_impl.cpp 
    connection_info_impl.cpp
    directory_ref_impl.cpp
    volume_ref_impl.cpp 
    eds_exception.cpp
    properties.cpp
    thumbnail.cpp
    )

target_link_libraries(camera_interface PUBLIC ${canon_edsdk} ${extra_libraries})
install(DIRECTORY "${canon_edsdk}" TYPE LIB)

target_include_directories(camera_interface
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    ${CanonEDSDK}/Header
    ${CMAKE_MODULE_PATH}
    ${Poco_INCLUDE_DIRS}
    )


add_library(camera_interface STATIC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_OSX_ARCHITECTURES x86_64)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})

find_package(Poco REQUIRED)

message(STATUS "Current source dir is '${CMAKE_SOURCE_DIR}'")

set(CanonEDSDK ${CMAKE_SOURCE_DIR}/CanonEDSDK/Machintosh/EDSDK)
message(STATUS "Canon EDSDK is at '${CanonEDSDK}'")
find_library(canon_edsdk NAMES EDSDK PATHS "${CanonEDSDK}/Framework" REQUIRED NO_DEFAULT_PATH)

add_compile_options(-Wall -Wextra -Wpedantic -Wshadow)
add_compile_options(-arch x86_64)

target_sources(camera_interface PRIVATE camera_info_impl camera_interface.cpp camera_list_impl.cpp 
    camera_ref_impl.cpp connection_info_impl.cpp directory_ref_impl.cpp volume_ref_impl.cpp 
    eds_exception.cpp properties.cpp thumbnail.cpp)

target_link_libraries(camera_interface PUBLIC ${canon_edsdk})
install(DIRECTORY "${canon_edsdk}" TYPE LIB)

target_include_directories(camera_interface
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    ${CanonEDSDK}/Header
    ${CMAKE_MODULE_PATH}
    ${Poco_INCLUDE_DIRS}
    )

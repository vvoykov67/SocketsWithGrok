cmake_minimum_required(VERSION 3.10)
project(SocketProject)

set(CMAKE_CXX_STANDARD 11)

# Define platform-specific include path
if(WIN32)
    set(PLATFORM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Include/Platforms/windows)
else()
    set(PLATFORM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Include/Platforms/unix)
endif()

# Include directories for all subprojects
include_directories(${CMAKE_SOURCE_DIR}/Include ${PLATFORM_INCLUDE_DIR})

add_subdirectory(Source)
add_subdirectory(Client)
add_subdirectory(Server)

project(Server)

add_executable(Server Server.cpp main.cpp)

target_include_directories(Server PUBLIC 
    ${CMAKE_SOURCE_DIR}/Include
    ${PLATFORM_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/Server/Modules
)

target_link_libraries(Server Common)

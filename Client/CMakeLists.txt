project(Client)

add_executable(Client main.cpp Client.cpp)

target_include_directories(Client PUBLIC 
    ${CMAKE_SOURCE_DIR}/Include
    ${PLATFORM_INCLUDE_DIR}
)

target_link_libraries(Client Common)

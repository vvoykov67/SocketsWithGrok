project(Common)

if(WIN32)
    set(PLATFORM_DIR Platforms/windows)
    set(PLATFORM_SOURCES
        ${PLATFORM_DIR}/Socket.cpp
        ${PLATFORM_DIR}/WinsockInitializer.cpp
    )
    set(LIBS ws2_32)
else()
    set(PLATFORM_DIR Platforms/unix)
    set(PLATFORM_SOURCES
        ${PLATFORM_DIR}/Socket.cpp
    )
    set(LIBS)
endif()

add_library(Common STATIC
    DiskFile.cpp
    ConfigReader.cpp
    Logger.cpp
    Buffer.cpp
    Error.cpp
    Message.cpp
    IpAddress.cpp
    SocketListener.cpp
    ${PLATFORM_SOURCES}
)

target_include_directories(Common PUBLIC 
    ${CMAKE_SOURCE_DIR}/Include
    ${CMAKE_SOURCE_DIR}/Include/${PLATFORM_DIR}
)

target_link_libraries(Common ${LIBS})

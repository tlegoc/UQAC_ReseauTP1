set(SOCKET_SOURCES
        Socket_common.cpp
)

if (WIN32)
    list(APPEND SOCKET_SOURCES
            Socket_windows.cpp
    )
else ()
    list(APPEND SOCKET_SOURCES
            Socket_posix.cpp
    )
endif ()

add_library(
        Socket
        ${SOCKET_SOURCES}
)

if(WIN32)
    target_link_libraries(Socket wsock32 ws2_32)
endif()

target_include_directories(
        Socket
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)
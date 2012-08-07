
set(IP_SOURCES
IpEndpointName.cpp
)

if(WIN32)
    list(APPEND IP_SOURCES
        win32/NetworkingUtils.cpp
        win32/UdpSocket.cpp
    )
endif(WIN32)

if(UNIX)
    list(APPEND IP_SOURCES
        posix/NetworkingUtils.cpp
        posix/UdpSocket.cpp
    )
endif(UNIX)

set(IP_HEADERS
IpEndpointName.h
NetworkingUtils.h
PacketListener.h
TimerListener.h
UdpSocket.h
)

ac_add_library(
    oscpackip oscpack/ip
    SOURCES ${IP_SOURCES}
    HEADERS ${IP_HEADERS}
    EXTERNS WINMM WINSOCK
    VERSION ${VERSION_STRING}
    SOVERSION ${VERSION_MAJOR}
    RUNTIME_INSTALL_COMPONENT ${RUNTIME_COMPONENT}
    DEVELOPMENT_INSTALL_COMPONENT ${DEVELOPMENT_COMPONENT}
)

_ac_add_include_path(oscpackip ${CMAKE_CURRENT_BINARY_DIR}/../include YES)

# stage our headers
foreach(HEADER ${IP_HEADERS})
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}
        ${CMAKE_CURRENT_BINARY_DIR}/../include/oscpack/ip/${HEADER}
        COPYONLY
    )
endforeach(HEADER ${IP_HEADERS})

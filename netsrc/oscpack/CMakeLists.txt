
cmake_minimum_required(VERSION 2.6)

project(OSCPACK)

include(AcMake)

include_directories(.) 

ac_add_project(
    OSCPACK 
    DEFINITIONS
        -DAC_BUILT_WITH_CMAKE
)

# XXX: fix this up
if(WIN32)
    set(WS2_32_LIBRARIES Ws2_32) # XXX: write find package
    set(WINMM_LIBRARIES Winmm) # XXX: write find package
    list(APPEND EXTERNS WS2_32 WINMM)
endif(WIN32)


set(OSC_SOURCES
osc/OscOutboundPacketStream.cpp
osc/OscPrintReceivedElements.cpp
osc/OscReceivedElements.cpp
osc/OscTypes.cpp
)

set(OSC_HEADERS
osc/MessageMappingOscPacketListener.h
osc/OscException.h
osc/OscHostEndianness.h
osc/OscOutboundPacketStream.h
osc/oscpack_settings.h
osc/OscPacketListener.h
osc/OscPrintReceivedElements.h
osc/OscReceivedElements.h
osc/OscTypes.h
)

ac_add_library(
    oscpackosc oscpack/osc/
    SOURCES ${OSC_SOURCES}
    HEADERS ${OSC_HEADERS}
    EXTERNS ${EXTERNS} # XXX: revisit when find-packages available
)


set(IP_SOURCES
ip/IpEndpointName.cpp
)

if(WIN32)
    list(APPEND IP_SOURCES
        ip/win32/NetworkingUtils.cpp
        ip/win32/UdpSocket.cpp
    )
endif(WIN32)

if(UNIX)
    list(APPEND IP_SOURCES
        ip/posix/NetworkingUtils.cpp
        ip/posix/UdpSocket.cpp
    )
endif(UNIX)

set(IP_HEADERS
ip/IpEndpointName.h
ip/NetworkingUtils.h
ip/PacketListener.h
ip/TimerListener.h
ip/UdpSocket.h
)

ac_add_library(
    oscpackip oscpack/ip
    SOURCES ${IP_SOURCES}
    HEADERS ${IP_HEADERS}
    EXTERNS ${EXTERNS} # XXX: revisit when find-packages available
)


# stage our headers
foreach(HEADER ${OSC_HEADERS} ${IP_HEADERS})
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}
        ${CMAKE_CURRENT_BINARY_DIR}/include/oscpack/${HEADER}
        COPYONLY
    )
endforeach(HEADER ${OSC_HEADERS} ${IP_HEADERS})

ac_end_project(OSCPACK)

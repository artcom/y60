
cmake_minimum_required(VERSION 2.6)

project(OSCPACK)

include(AcMake)

include_directories(.) 

ac_add_project(
    OSCPACK 
    DEFINITIONS
        -DAC_BUILT_WITH_CMAKE
)

set(OSC_SOURCES
osc/OscOutboundPacketStream.cpp
osc/OscPrintReceivedElements.cpp
osc/OscReceivedElements.cpp
osc/OscTypes.cpp
)

set(OSC_IP_SOURCES
ip/IpEndpointName.cpp
)

if(WIN32)
    list(APPEND OSC_IP_SOURCES
        ip/win32/NetworkingUtils.cpp
        ip/win32/UdpSocket.cpp
    )
endif(WIN32)

if(UNIX)
    list(APPEND OSC_IP_SOURCES 
        ip/posix/NetworkingUtils.cpp
        ip/posix/UdpSocket.cpp
    )
endif(UNIX)

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

set(OSC_IP_HEADERS
ip/IpEndpointName.h
ip/NetworkingUtils.h
ip/PacketListener.h
ip/TimerListener.h
ip/UdpSocket.h
)

set(SOURCES ${OSC_SOURCES} ${OSC_IP_SOURCES})
set(HEADERS ${OSC_HEADERS} ${OSC_IP_HEADERS})

# XXX: ingo, stage headers separately for both libs
#      and teach acmake to install them with their
#      relative paths or something similar.

if(WIN32)
    set(WS2_32_LIBRARIES Ws2_32) # XXX: write find package
    set(WINMM_LIBRARIES Winmm) # XXX: write find package
    list(APPEND EXTERNS WS2_32 WINMM)
endif(WIN32)

ac_add_library(
   oscpack oscpack/osc/
   SOURCES ${SOURCES}
   HEADERS ${HEADERS}
   EXTERNS ${EXTERNS}
)


ac_end_project(OSCPACK)

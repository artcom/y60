
cmake_minimum_required(VERSION 2.6)

project(OSCPACK)

# XXX: definition handling
add_definitions(-DAC_BUILT_WITH_CMAKE)

include(AcMake)

include_directories(.) 

set( SOURCES
OscOutboundPacketStream.cpp
OscPrintReceivedElements.cpp
OscReceivedElements.cpp
OscTypes.cpp
../ip/IpEndpointName.cpp
)

if(WIN32)
    list( APPEND SOURCES
        ../ip/win32/NetworkingUtils.cpp
        ../ip/win32/UdpSocket.cpp
        )
else(WIN32)
    list( APPEND SOURCES 
        ../ip/posix/NetworkingUtils.cpp
        ../ip/posix/UdpSocket.cpp
        )
endif(WIN32)

if(LINUX)
    add_definitions(-DOSC_HOST_LITTLE_ENDIAN)
endif(LINUX)


set( HEADERS
MessageMappingOscPacketListener.h
OscException.h
OscHostEndianness.h
OscOutboundPacketStream.h
oscpack_settings.h
OscPacketListener.h
OscPrintReceivedElements.h
OscReceivedElements.h
OscTypes.h
../ip/IpEndpointName.h
../ip/NetworkingUtils.h
../ip/PacketListener.h
../ip/TimerListener.h
../ip/UdpSocket.h
)

# stage our header
foreach(A_HEADER ${HEADERS})
  configure_file(
     ${CMAKE_CURRENT_SOURCE_DIR}/${A_HEADER}
     ${CMAKE_CURRENT_BINARY_DIR}/include/oscpack/osc/${A_HEADER}
     COPYONLY
  )
endforeach(A_HEADER)

ac_add_project(
    OSCPACK 
)

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

ac_export_project(OSCPACK)


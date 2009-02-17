
set(OSC_SOURCES
OscOutboundPacketStream.cpp
OscPrintReceivedElements.cpp
OscReceivedElements.cpp
OscTypes.cpp
)

set(OSC_HEADERS
oscpack_settings.h
MessageMappingOscPacketListener.h
OscException.h
OscHostEndianness.h
OscOutboundPacketStream.h
oscpack_settings.h
OscPacketListener.h
OscPrintReceivedElements.h
OscReceivedElements.h
OscTypes.h
)

ac_add_library(
    oscpackosc oscpack/osc/
    SOURCES ${OSC_SOURCES}
    HEADERS ${OSC_HEADERS}
    EXTERNS ${EXTERNS} # XXX: revisit when find-packages available
)

# stage headers
foreach(HEADER ${OSC_HEADERS})
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}
        ${CMAKE_CURRENT_BINARY_DIR}/../include/oscpack/osc/${HEADER}
        COPYONLY
    )
endforeach(HEADER ${OSC_HEADERS})

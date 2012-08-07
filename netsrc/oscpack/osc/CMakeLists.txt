
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
    VERSION ${VERSION_STRING}
    SOVERSION ${VERSION_MAJOR}
    RUNTIME_INSTALL_COMPONENT ${RUNTIME_COMPONENT}
    DEVELOPMENT_INSTALL_COMPONENT ${DEVELOPMENT_COMPONENT}
)

_ac_add_include_path(oscpackosc ${CMAKE_CURRENT_BINARY_DIR}/../include YES)

# stage headers
foreach(HEADER ${OSC_HEADERS})
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}
        ${CMAKE_CURRENT_BINARY_DIR}/../include/oscpack/osc/${HEADER}
        COPYONLY
    )
endforeach(HEADER ${OSC_HEADERS})

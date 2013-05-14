
set(SOURCES
ASSDriver.cpp
ASSUtils.cpp
DummyTransport.cpp
SerialTransport.cpp
SocketTransport.cpp
TransportLayer.cpp
UDPTransport.cpp
)

set(HEADERS
ASSDriver.h
ASSUtils.h
Cursor.h
DummyTransport.h
SerialTransport.h
SocketTransport.h
TransportLayer.h
y60_asscore_settings.h
UDPTransport.h
)

list(APPEND EXTERNALS GLEW)

ac_add_library(
    ASSCore y60/components/ASSDriver/ASSCore
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS asldom aslmath y60jsbase y60jslib y60image y60scene 
    EXTERNS ${EXTERNALS}
    VERSION ${VERSION_STRING}
    SOVERSION ${VERSION_MAJOR}
    RUNTIME_INSTALL_COMPONENT ASS_runtime
    DEVELOPMENT_INSTALL_COMPONENT ASS_development
)

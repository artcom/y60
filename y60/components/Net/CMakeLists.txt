
set(SOURCES
JSNetPlugIn.cpp
JSSocket.cpp
JSTCPServer.cpp
)

set(HEADERS
JSSocket.h
JSTCPServer.h
y60_net_settings.h
)

y60_add_component(
    Net
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslinet y60jslib
)

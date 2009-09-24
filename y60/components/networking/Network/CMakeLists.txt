
set(SOURCES
JSNetPlugIn.cpp
JSSocket.cpp
JSTCPServer.cpp
JSFrameSocket.cpp
)

set(HEADERS
JSSocket.h
JSTCPServer.h
JSFrameSocket.h
y60_net_settings.h
)

y60_add_component(
    Network
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslinet y60jslib
)

y60_add_jstest(Socket mod_Net)

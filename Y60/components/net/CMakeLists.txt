
set(SOURCES
JSNetPlugIn.cpp
JSSocket.cpp
JSTCPServer.cpp
)

set(HEADERS
JSSocket.h
JSTCPServer.h
)

y60_add_component(
    net
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS OPENEXR SPIDERMONKEY
)



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

list( APPEND EXTERNALS SPIDERMONKEY OPENEXR NVIDIA_CG)

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

y60_add_component(
    net
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslinet y60jslib
    EXTERNS ${EXTERNALS}
)


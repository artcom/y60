
set(SOURCES
ASSTuioServer.cpp
)

set(HEADERS
ASSTuioServer.h
)

set(EXTERNS
TUIO
)

if(WIN32)
    list(APPEND EXTERNS WINMM)
endif(WIN32)

y60_add_component(
    ASSTuioServer
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib ASSCore
    EXTERNS ${EXTERNS}
    RUNTIME_INSTALL_COMPONENT ASS_runtime
    DEVELOPMENT_INSTALL_COMPONENT ASS_development
)

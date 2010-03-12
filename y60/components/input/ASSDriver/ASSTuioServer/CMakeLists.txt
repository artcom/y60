
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

y60_begin_application(
    proximatrixtuio
    DISPLAY_NAME "Proximatrix TUIO Server"
    DESCRIPTION  "TUIO server for the Proximatrix"
    DEPENDS ASS_runtime Y60_runtime
)

y60_add_asset(server.js)
y60_add_asset(settings.xml)

y60_add_launcher(
    Proximatrix_TUIO_Server
    COMMAND_NAME proximatrix-tuio
    DESCRIPTION  "TUIO server for the Proximatrix"
    ENGINE y60
    MAIN_SCRIPT server.js
    BUILD_WORKING_DIR SOURCE
    INSTALL_WORKING_DIR ASSETS
    CATEGORIES
        Proximatrix
)

y60_end_application(proximatrixtuio)

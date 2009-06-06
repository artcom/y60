
set(SOURCES
ASSEventSource.cpp
)

set(HEADERS
ASSEventSource.h
y60_eventsource_settings.h
)

list(APPEND EXTERNALS GLEW)

y60_add_component(
    y60ASSEventSource
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase asldom aslmath y60ASSCore y60input y60jsbase 
    EXTERNS ${EXTERNALS}
    RUNTIME_INSTALL_COMPONENT ASS_runtime
    DEVELOPMENT_INSTALL_COMPONENT ASS_development
)

y60_begin_application(
    proximatrixtools
    DISPLAY_NAME "Proximatrix Tools"
    DESCRIPTION  "Tools for setting up and debugging the ART+COM Proximatrix"
    DEPENDS ASS_runtime Y60_runtime
    INSTALL_TYPES runtime development
)

y60_add_asset(demo.js)
y60_add_asset(dump.js)
y60_add_asset(settings.xml)

y60_add_launcher(
    "Proximatrix Analyzer"
    COMMAND_NAME proximatrix-analyzer
    DESCRIPTION "Application for displaying Proximatrix sensor input"
    ENGINE y60
    MAIN_SCRIPT demo.js
    BUILD_WORKING_DIR SOURCE
    CATEGORIES
        Proximatrix
)

y60_add_launcher(
    "Proximatrix Dumper"
    COMMAND_NAME proximatrix-dump
    DESCRIPTION "Application for dumping Proximatrix events"
    ENGINE y60
    MAIN_SCRIPT dump.js
    BUILD_WORKING_DIR SOURCE
    CATEGORIES
        Proximatrix
)

y60_end_application(proximatrixtools)

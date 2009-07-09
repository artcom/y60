
set(SOURCES
ASSOscClient.cpp
ASSOscReceiver.cpp
ASSOscPlugin.cpp
JSASSOscClient.cpp
)

set(HEADERS
ASSOscClient.h
ASSOscReceiver.h
JSASSOscClient.h
y60_oscclient_settings.h
)

y60_add_component(
    ASSOscClient
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib ASSCore oscpackip oscpackosc
    RUNTIME_INSTALL_COMPONENT ASS_runtime
    DEVELOPMENT_INSTALL_COMPONENT ASS_development
)

y60_begin_application(
    proximatrixosc
    DISPLAY_NAME "Proximatrix OSC Client"
    DESCRIPTION  "OSC touch event sender for the ART+COM Proximatrix"
    DEPENDS ASS_runtime Y60_runtime
    INSTALL_TYPES runtime development
)

y60_add_asset(client.js)
y60_add_asset(settings.xml)

y60_add_launcher(
    "Proximatrix OSC Sender"
    COMMAND_NAME proximatrix-osc
    DESCRIPTION  "OSC touch event sender for the ART+COM Proximatrix"
    ENGINE y60
    MAIN_SCRIPT client.js
    BUILD_WORKING_DIR SOURCE
    INSTALL_WORKING_DIR ASSETS
    CATEGORIES
        Proximatrix
)

y60_end_application(proximatrixosc)

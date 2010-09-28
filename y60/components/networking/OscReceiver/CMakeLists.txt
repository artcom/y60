
set(SOURCES
JSOscReceiver.cpp
osceventxsd.cpp
OscReceiver.cpp
OscReceiverPlugin.cpp
)

set(HEADERS
JSOscReceiver.h
OscReceiver.h
OscReceiverPlugin.h
)

list ( APPEND EXTERNALS OPENEXR OSCPACK )

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

y60_add_component(
    OscReceiver
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS asldom y60jsbase y60input oscpackip oscpackosc
    EXTERNS ${EXTERNALS}
)

asl_add_schema(
    oscevent.xsd
    CXX_FILE      osceventxsd.cpp
    CXX_NAMESPACE y60
    CXX_VARNAME   ourosceventxsd
    CXX_INCLUDE   osceventxsd.h
)

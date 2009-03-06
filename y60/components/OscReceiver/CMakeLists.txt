
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
    y60OscReceiver
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS asldom y60jsbase y60input oscpackip oscpackosc
    EXTERNS ${EXTERNALS}
)

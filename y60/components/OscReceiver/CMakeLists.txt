
set(SOURCES
JSOscReceiver.cpp
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
    DEPENDS asldom y60jsbase y60input
    EXTERNS ${EXTERNALS}
)

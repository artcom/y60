
set(SOURCES
JSOscReceiver.cpp
OscReceiver.cpp
OscReceiverPlugin.cpp
osceventxsd.cpp
)

set(HEADERS
JSOscReceiver.h
OscReceiver.h
OscReceiverPlugin.h
)

y60_add_component(
    OscReceiver
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS OPENEXR SPIDERMONKEY
)


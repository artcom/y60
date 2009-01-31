
set(SOURCES
JSOscSender.cpp
JSOscSenderPlugIn.cpp
OscSender.cpp
)

set(HEADERS
JSOscSender.h
OscSender.h
)

y60_add_component(
    OscSender
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
)

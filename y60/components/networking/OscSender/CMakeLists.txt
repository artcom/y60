
set(SOURCES
JSOscSender.cpp
JSOscSenderPlugIn.cpp
OscSender.cpp
)

set(HEADERS
JSOscSender.h
OscSender.h
)

list ( APPEND EXTERNALS OSCPACK )

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

y60_add_component(
    OscSender
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS asldom aslnet y60jsbase oscpackip oscpackosc
    EXTERNS ${EXTERNALS}
)

y60_add_jstest(OscSender mod_OscSender)


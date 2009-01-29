
set(SOURCES
ASSEventSource.cpp
)

set(HEADERS
ASSEventSource.h
y60_eventsource_settings.h
)

list( APPEND EXTERNALS OPENEXR GLEW )   

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

y60_add_component(
    ASSEventSource
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase asldom aslmath y60ASSCore y60input y60jsbase 
    EXTERNS ${EXTERNALS}
)


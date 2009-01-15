
set(SOURCES
ASSEventSource.cpp
)

set(HEADERS
ASSEventSource.h
start.sh
y60_eventsource_settings.h
)

list( APPEND EXTERNALS OPENEXR GLEW )   

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

y60_add_component(
    EventSource
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase asldom aslmath ASSCore y60input y60jsbase 
    EXTERNS ${EXTERNALS}
)


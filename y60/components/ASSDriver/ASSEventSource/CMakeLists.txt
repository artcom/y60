
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
)

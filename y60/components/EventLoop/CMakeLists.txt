
set(SOURCES
JSEventLoop.cpp
)

set(HEADERS
JSEventLoop.h
y60_eventloop_settings.h
)

list( APPEND EXTERNALS SPIDERMONKEY OPENEXR NVIDIA_CG )

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

y60_add_component(
    EventLoop
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS ${EXTERNALS}
)


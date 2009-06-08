
set(SOURCES
JSEventLoop.cpp
)

set(HEADERS
JSEventLoop.h
y60_eventloop_settings.h
)

y60_add_component(
    y60EventLoop
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
)

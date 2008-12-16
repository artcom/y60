
set(SOURCES
JSEventLoop.cpp
)

set(HEADERS
JSEventLoop.h
)

y60_add_component(
    EventLoop
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS OPENEXR SPIDERMONKEY
)


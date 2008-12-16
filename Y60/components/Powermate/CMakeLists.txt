
set(SOURCES
PowermateInputExtension.cpp
)

set(HEADERS
PowermateInputExtension.h
)

y60_add_component(
    Powermate
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS OPENEXR SPIDERMONKEY
)


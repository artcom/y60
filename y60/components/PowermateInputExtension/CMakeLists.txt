
set(SOURCES
PowermateInputExtension.cpp
)

set(HEADERS
PowermateInputExtension.h
)

y60_add_component(
    PowermateInputExtension
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS SPIDERMONKEY
)


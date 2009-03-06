
set(SOURCES
PowermateInputExtension.cpp
)

set(HEADERS
PowermateInputExtension.h
)

y60_add_component(
    y60PowermateInputExtension
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
)

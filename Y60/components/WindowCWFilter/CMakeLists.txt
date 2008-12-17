
set(SOURCES
PLFilterWindowCW.cpp
WindowCWPlugin.cpp
)

set(HEADERS
PLFilterWindowCW.h
)

y60_add_component(
    WindowCWFilter
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60image
    EXTERNS PAINTLIB
)


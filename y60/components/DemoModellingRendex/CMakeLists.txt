
set(SOURCES
DemoModellingRendex.cpp
)

set(HEADERS
y60_demomodellingrendex_settings.h
)

y60_add_component(
    DemoModellingRendex
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
)

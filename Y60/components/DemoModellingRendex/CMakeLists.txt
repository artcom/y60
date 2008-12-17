
set(SOURCES
DemoModellingRendex.cpp
)

set(HEADERS
)

y60_add_component(
    DemoModellingRendex
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS SPIDERMONKEY
)


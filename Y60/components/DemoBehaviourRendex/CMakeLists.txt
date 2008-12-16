
set(SOURCES
DemoBehaviour.cpp
)

set(HEADERS
DemoBehaviour.h
)

y60_add_component(
    DemoBehaviourRendex
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS OPENEXR SPIDERMONKEY
)


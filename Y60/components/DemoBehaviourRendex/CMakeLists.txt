
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
    DEPENDS y60jslib
    EXTERNS SPIDERMONKEY OPENEXR GLEW
)


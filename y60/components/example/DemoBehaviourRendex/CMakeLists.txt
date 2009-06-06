
set(SOURCES
DemoBehaviour.cpp
)

set(HEADERS
DemoBehaviour.h
y60_demobehaviourrendex_settings.h
)

y60_add_component(
    y60DemoBehaviourRendex
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
)

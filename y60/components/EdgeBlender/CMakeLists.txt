
set(SOURCES
EdgeBlender.cpp
)

set(HEADERS
y60_edgeblender_settings.h
)

y60_add_component(
    y60EdgeBlender
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
)

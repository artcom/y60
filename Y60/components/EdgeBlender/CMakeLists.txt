
set(SOURCES
EdgeBlender.cpp
)

set(HEADERS
)

y60_add_component(
    EdgeBlender
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS SPIDERMONKEY OPENGL
)


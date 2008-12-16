
set(SOURCES
EdgeBlender.cpp
)

set(HEADERS
)

y60_add_component(
    EdgeBlender
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS OPENEXR SPIDERMONKEY
)


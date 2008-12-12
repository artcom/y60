
set(SOURCES
testRendererExtension.cpp
)

set(HEADERS
)

y60_add_component(
    testRendererExtension
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS OPENEXR
)



set(SOURCES
DemoRendererExtension.cpp
)

set(HEADERS
)

y60_add_component(
    DemoRendererExtension
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS OPENEXR
)



set(SOURCES
JSGLBindingPlugin.cpp
)

set(HEADERS
)

y60_add_component(
    GLBinding
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS OPENEXR
)


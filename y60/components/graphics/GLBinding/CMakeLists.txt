
set(SOURCES
JSGLBindingPlugin.cpp
)

set(HEADERS
y60_glbinding_settings.h
)

y60_add_component(
    y60GLBinding
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS GLEW
)

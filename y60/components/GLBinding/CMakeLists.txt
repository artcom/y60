
set(SOURCES
JSGLBindingPlugin.cpp
)

set(HEADERS
y60_glbinding_settings.h
)

y60_add_component(
    GLBinding
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS SPIDERMONKEY OPENGL GLEW OPENEXR
)


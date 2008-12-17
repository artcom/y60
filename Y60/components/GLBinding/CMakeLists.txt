
set(SOURCES
JSGLBindingPlugin.cpp
)

set(HEADERS
)

y60_add_component(
    GLBinding
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS SPIDERMONKEY
)


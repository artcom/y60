
set(SOURCES
DemoRendererExtension.cpp
)

set(HEADERS
y60_demorendererextension_settings.h
)

y60_add_component(
    DemoRendererExtension
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS SPIDERMONKEY OPENEXR GLEW
)


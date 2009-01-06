
set(SOURCES
CairoUtilities.cpp
JSCairo.cpp
JSCairoPattern.cpp
JSCairoPlugin.cpp
JSCairoSurface.cpp
)

set(HEADERS
CairoUtilities.h
CairoWrapper.h
JSCairo.h
JSCairoPattern.h
JSCairoSurface.h
)

y60_add_component(
    Cairo
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib y60input
    EXTERNS SPIDERMONKEY GLEW OPENEXR CAIRO
)


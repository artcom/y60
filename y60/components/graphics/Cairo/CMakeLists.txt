
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
CairoWrapper.impl
JSCairo.h
JSCairoPattern.h
JSCairoSurface.h
)

y60_add_component(
    y60Cairo
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib y60input
    EXTERNS CAIRO
)

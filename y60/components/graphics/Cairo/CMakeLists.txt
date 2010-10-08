
set(SOURCES
CairoUtilities.cpp
JSContext.cpp
JSPattern.cpp
JSCairoPlugin.cpp
JSSurface.cpp
)

set(HEADERS
CairoUtilities.h
CairoWrapper.h
CairoWrapper.impl
JSContext.h
JSPattern.h
JSSurface.h
)

y60_add_component(
    Cairo
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib y60input
    EXTERNS CAIRO
)

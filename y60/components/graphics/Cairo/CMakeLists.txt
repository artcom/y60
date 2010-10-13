
set(SOURCES
CairoUtilities.cpp
JSCairoContext.cpp
JSCairoPattern.cpp
JSCairoPlugin.cpp
JSPangoFontDescription.cpp
JSPangoLayout.cpp
JSCairoSurface.cpp
)

set(HEADERS
CairoUtilities.h
CairoWrapper.h
CairoWrapper.impl
JSCairoContext.h
JSCairoPattern.h
JSPangoFontDescription.h
JSPangoLayout.h
JSCairoSurface.h
)

y60_add_component(
    Cairo
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib y60input
    EXTERNS CAIRO PANGO PANGOCAIRO
)

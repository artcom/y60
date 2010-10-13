
set(SOURCES
CairoUtilities.cpp
JSCairoContext.cpp
JSCairoPattern.cpp
JSCairoPlugin.cpp
JSCairoSurface.cpp

JSPangoContext.cpp
JSPangoFontDescription.cpp
JSPangoLayout.cpp
)

set(HEADERS
CairoUtilities.h
CairoWrapper.h
CairoWrapper.impl
JSCairoContext.h
JSCairoPattern.h
JSCairoSurface.h

JSPangoContext.h
JSPangoFontDescription.h
JSPangoLayout.h
)

y60_add_component(
    Cairo
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib y60input
    EXTERNS CAIRO PANGO PANGOCAIRO
)


set(SOURCES
JSPangoContext.cpp
JSPangoFontDescription.cpp
JSPangoLayout.cpp
JSPangoPlugin.cpp
)

set(HEADERS
JSPangoContext.h
JSPangoFontDescription.h
JSPangoLayout.h
)

y60_add_component(
    Pango
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib y60input
    EXTERNS CAIRO PANGO PANGOCAIRO
)

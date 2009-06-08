
set(SOURCES
GlurFilter.cpp
GlurPlugin.cpp
)

set(HEADERS
GlurFilter.h
y60_glurfilter_settings.h
)

y60_add_component(
    y60GlurFilter
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60image
)

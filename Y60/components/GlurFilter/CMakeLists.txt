
set(SOURCES
GlurFilter.cpp
GlurPlugin.cpp
)

set(HEADERS
GlurFilter.h
)

y60_add_component(
    GlurFilter
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS OPENEXR SPIDERMONKEY
)


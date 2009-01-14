
set(SOURCES
GlurFilter.cpp
GlurPlugin.cpp
)

set(HEADERS
GlurFilter.h
y60_glurfilter_settings.h
)

list( APPEND EXTERNALS SPIDERMONKEY OPENEXR )

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

y60_add_component(
    GlurFilter
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60image
    EXTERNS ${EXTERNALS}
)


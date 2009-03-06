
set(SOURCES
PLFilterWindowCW.cpp
WindowCWPlugin.cpp
)

set(HEADERS
PLFilterWindowCW.h
y60_windowcwfilter_settings.h
)

list( APPEND EXTERNALS PAINTLIB OPENEXR )

y60_add_component(
    y60WindowCW
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60image
    EXTERNS ${EXTERNALS}
)



set(SOURCES
PLFilterWindowCW.cpp
WindowCWPlugin.cpp
)

set(HEADERS
PLFilterWindowCW.h
y60_windowcwfilter_settings.h
)

list( APPEND EXTERNALS PAINTLIB OPENEXR )

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

y60_add_component(
    WindowCW
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60image
    EXTERNS ${EXTERNALS}
)


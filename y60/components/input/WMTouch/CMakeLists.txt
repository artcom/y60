
set(SOURCES
    WMTouchPlugin.cpp
)

set(HEADERS
y60_wmtouch_settings.h
)

set(EXTERNS SDL)

if(WIN32)
endif(WIN32)

y60_add_component(
    WMTouch
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS ${EXTERNS}
)


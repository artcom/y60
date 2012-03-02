
set(SOURCES
    WMTouchPlugin.cpp
    wmtoucheventxsd.cpp
)

set(HEADERS
    WMTouchPlugin.h
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

asl_add_schema(
    wmtouchevent.xsd
    CXX_FILE      wmtoucheventxsd.cpp
    CXX_NAMESPACE y60
    CXX_VARNAME   ourwmtoucheventxsd
    CXX_INCLUDE   wmtoucheventxsd.h
)



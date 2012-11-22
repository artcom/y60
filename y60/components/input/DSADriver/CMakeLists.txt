
set(SOURCES
JSDSADriverPlugin.cpp
JSDSADriver.cpp
DSADriver.cpp
SensorArray.cpp
SensorServer.cpp
)

set(HEADERS
JSDSADriver.h
DSADriver.h
SensorArray.h
SensorServer.h
y60_dsadriver_settings.h
)

if(NOT WIN32)
    find_package(Boost COMPONENTS regex)
endif(NOT WIN32)

list(APPEND EXTERNALS Boost)
y60_add_component(
    DSADriver
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS ${EXTERNALS}
)


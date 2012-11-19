
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

y60_add_component(
    DSADriver
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
)


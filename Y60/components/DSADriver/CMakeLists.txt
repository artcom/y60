
set(SOURCES
DSADriver.cpp
SensorArray.cpp
SensorServer.cpp
)

set(HEADERS
DSADriver.h
SensorArray.h
SensorServer.h
)

y60_add_component(
    DSADriver
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS OPENEXR
)


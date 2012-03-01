
set(SOURCES
TimeSyncBarrier.cpp
)

set(HEADERS
y60_TimeSyncBarrier_settings.h
)

y60_add_component(
    TimeSyncBarrier 
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib ${JSDEPENDS}
    EXTERNS  ${JSEXTERN}
)

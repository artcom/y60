
set(SOURCES
DvbTeleText.cpp
DvbTuner.cpp
JSDvbPlugin.cpp
JSDvbTuner.cpp
)

set(HEADERS
DvbTeleText.h
DvbTuner.h
JSDvbTuner.h
)

y60_add_component(
    DVB
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS SPIDERMONKEY
)


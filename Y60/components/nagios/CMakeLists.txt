
set(SOURCES
NagiosPlugin.cpp
StatusServer.cpp
)

set(HEADERS
StatusServer.h
)

y60_add_component(
    nagios
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS SPIDERMONKEY
)



set(SOURCES
NagiosPlugin.cpp
StatusServer.cpp
)

set(HEADERS
StatusServer.h
y60_nagios_settings.h
)

y60_add_component(
    Nagios
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS SPIDERMONKEY GLEW OPENEXR
)



set(SOURCES
JSSynergyServer.cpp
SynergyServer.cpp
TestSynergyServer.cpp
)

set(HEADERS
JSSynergyServer.h
SynergyServer.h
TestSynergyServer.h
y60_synergyserver_settings.h
)

y60_add_component(
    SynergyServer
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
)

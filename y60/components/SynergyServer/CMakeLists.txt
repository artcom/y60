
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

list( APPEND EXTERNALS SPIDERMONKEY OPENEXR NVIDIA_CG )   

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

y60_add_component(
    SynergyServer
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS ${EXTERNALS}
)


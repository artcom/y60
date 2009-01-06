
# XXXX: LINUX ONLY. WINDOWS TERRIBLY BROKEN.

set(SOURCES
Liimote.cpp
WiimoteDriver.cpp
WiiRemote.cpp
WiiScanner.cpp
eventxsd.cpp
)

set(HEADERS
Liimote.h
WiiEvent.h
WiimoteDriver.h
WiiRemote.h
WiiScanner.h
)

y60_add_component(
    Wiimote
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib y60input
    EXTERNS SPIDERMONKEY GLEW OPENEXR
)

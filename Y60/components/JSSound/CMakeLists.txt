
set(SOURCES
JSGrainSource.cpp
JSSound.cpp
JSSoundManager.cpp
)

set(HEADERS
JSGrainSource.h
JSSound.h
JSSoundManager.h
)

y60_add_component(
    JSSound
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS OPENEXR
)


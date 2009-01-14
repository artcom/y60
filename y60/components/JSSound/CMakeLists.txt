
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

list( APPEND EXTERNALS SPIDERMONKEY OPENEXR AVFORMAT NVIDIA_CG)

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

y60_add_component(
    y60JSSound
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib y60sound
    EXTERNS ${EXTERNALS}
)


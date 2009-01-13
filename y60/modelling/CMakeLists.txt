
set(SOURCES
SimpleTesselator.cpp
modelling_functions.cpp
)

set(HEADERS
SimpleTesselator.h
VertexIndexBuilder.h
modelling_functions.h
y60_modelling_settings.h
)

set(TESTS
ModellingFunctions
)

list( APPEND EXTERNALS OPENEXR GLEW)

if(OSX)
endif(OSX)

if(LINUX)
endif(LINUX)

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

ac_add_library(
    y60modelling y60/modelling
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase aslmath asldom aslzip y60base y60image y60video y60scene
    EXTERNS ${EXTERNALS}
    TESTS   ${TESTS}
)


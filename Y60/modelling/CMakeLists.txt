
set(SOURCES
SimpleTesselator.cpp
modelling_functions.cpp
)

set(HEADERS
SimpleTesselator.h
VertexIndexBuilder.h
modelling_functions.h
)

set(TESTS
ModellingFunctions
)

if(OSX)
endif(OSX)

if(LINUX)
endif(LINUX)

if(WIN32)
endif(WIN32)

ac_add_library(
    y60modelling y60/modelling
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase aslmath asldom aslzip y60base y60image y60video y60scene paintlib
    EXTERNS OPENEXR
    TESTS   ${TESTS}
)


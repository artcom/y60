
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

ac_add_library(
    y60modelling y60/modelling
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase aslmath asldom aslzip y60base y60image y60video y60scene
    TESTS   ${TESTS}
    VERSION ${VERSION_STRING}
    SOVERSION ${VERSION_MAJOR}
    RUNTIME_INSTALL_COMPONENT ${RUNTIME_COMPONENT}
    DEVELOPMENT_INSTALL_COMPONENT ${DEVELOPMENT_COMPONENT}
)

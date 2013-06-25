
set(SOURCES
    Proximatrix2.cpp
    proximatrix2toucheventxsd.cpp    
)

set(HEADERS
)

set(EXTERNS
)

y60_add_component(
    Proximatrix2 
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS ${EXTERNS}
)

asl_add_schema(
    proximatrix2touchevent.xsd
    CXX_FILE      proximatrix2toucheventxsd.cpp
    CXX_NAMESPACE y60
    CXX_VARNAME   ourproximatrix2toucheventxsd
    CXX_INCLUDE   proximatrix2toucheventxsd.h
)


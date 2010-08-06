
set(SOURCES
Gesture.cpp
gestureeventxsd.cpp
)

set(HEADERS
Gesture.h
gestureeventxsd.h
)



y60_add_component(
    Gesture
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib y60input 
    
)
asl_add_schema(
    gestureevent.xsd
    CXX_FILE      gestureeventxsd.cpp
    CXX_NAMESPACE y60
    CXX_VARNAME   ourgestureeventxsd
    CXX_INCLUDE   gestureeventxsd.h
)

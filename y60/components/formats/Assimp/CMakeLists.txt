
set(SOURCES
    JSAssimpPlugin.cpp
    JSAssimpLoader.cpp
    AssimpLoader.cpp
)

set(HEADERS
    JSAssimpLoader.h
    AssimpLoader.h
)

# that line confuses me :-/ , because it should get called automatically by y60_add_component, right?!
find_package(Assimp) 

y60_add_component(
    AssimpLoader 
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib y60jsscene
    EXTERNS ASSIMP
)


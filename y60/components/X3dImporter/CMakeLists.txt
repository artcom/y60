
set(SOURCES
X3dImport.cpp
)

set(HEADERS
X3dImport.h
)

y60_add_component(
    X3dImporter
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS PAINTLIB OPENEXR
)


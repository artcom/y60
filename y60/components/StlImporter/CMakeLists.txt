
set(SOURCES
JSStlCodec.cpp
StlCodec.cpp
StlImport.cpp
)

set(HEADERS
JSStlCodec.h
StlCodec.h
StlImport.h
y60_stlimporter_settings.h
)

y60_add_component(
    y60StlImporter
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib ${JSDEPENDS}
    EXTERNS ${JSEXTERN} GLEW OPENEXR
)


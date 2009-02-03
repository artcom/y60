
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
    StlImporter
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
#    EXTERNS SPIDERMONKEY GLEW OPENEXR
    EXTERNS MOZJS GLEW OPENEXR
)


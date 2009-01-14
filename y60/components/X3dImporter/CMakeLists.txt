
set(SOURCES
X3dImport.cpp
)

set(HEADERS
X3dImport.h
y60_x3dimporter_settings.h
)

list( APPEND EXTERNALS PAINTLIB OPENEXR )

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

y60_add_component(
    X3dImporter
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS ${EXTERNALS}
)


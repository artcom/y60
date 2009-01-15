
set(SOURCES
ASSSchema.cpp
)

set(HEADERS
y60_assschema_settings.h
)

list( APPEND EXTERNALS OPENEXR GLEW )   

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

y60_add_component(
    ASSSchema
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS asldom y60input y60jsbase
    EXTERNS ${EXTERNALS}
)


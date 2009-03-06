
set(SOURCES
ASSSchema.cpp
)

set(HEADERS
y60_assschema_settings.h
)

y60_add_component(
    y60ASSSchema
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS asldom y60input y60jsbase
    EXTERNS ${EXTERNALS}
)

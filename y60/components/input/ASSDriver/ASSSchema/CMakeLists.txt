
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
    RUNTIME_INSTALL_COMPONENT ASS_runtime
    DEVELOPMENT_INSTALL_COMPONENT ASS_development
)

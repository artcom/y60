set(SOURCES
DInputExtension.cpp
)

set(HEADERS
DInputExtension.h
y60_dinput_settings.h
)

set(EXTERNALS DIRECTX SDL)

y60_add_component(
    DInput
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS ${EXTERNALS}
)

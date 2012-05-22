
set(SOURCES
testRendererExtension.cpp
)

set(HEADERS
y60_testrendererextension_settings.h
)

y60_add_component(
    testRendererExtension
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib ${JSDEPENDS}
    EXTERNS  GLEW ${JSEXTERN}
    DONT_INSTALL
)

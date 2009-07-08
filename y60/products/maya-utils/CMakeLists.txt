set(SOURCES
ColorAtPointHDR.cpp
KSAnimationSampler.cpp
)

set(HEADERS
KSAnimationSampler.h
)

# XXX: add definitions for compiling maya plugin
add_definitions(${MAYA_PLUGIN_DEFINITIONS})

ac_add_plugin(
    maya-utils y60/maya-utils
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60base
    EXTERNS MAYA OPENEXR
    DONT_INSTALL
)


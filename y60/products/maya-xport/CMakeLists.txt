
set(SOURCES
AnimationExporter.cpp
CurveExporter.cpp
LightExporter.cpp
MaterialExporter.cpp
MayaHelpers.cpp
ProgressBar.cpp
SceneExporter.cpp
ShapeExporter.cpp
)

set(HEADERS
AnimationExporter.h
CurveExporter.h
ExportExceptions.h
JointWeight.h
LightExporter.h
MaterialExporter.h
MayaHelpers.h
ProgressBar.h
SceneExporter.h
ShapeExporter.h
)

# XXX: add definitions for compiling maya plugin
add_definitions(${MAYA_PLUGIN_DEFINITIONS})

ac_add_plugin(
    maya-xport y60/maya-xport
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60scene y60modelling
    EXTERNS MAYA
    DONT_INSTALL
)

get_property(MAYA_XPORT_PLUGIN_LOCATION TARGET maya-xport PROPERTY LOCATION)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/install.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/install.sh
    @ONLY
    NEWLINE_STYLE LF
)

set(MAYA_XPORT_PLUGIN_LOCATION)


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
)


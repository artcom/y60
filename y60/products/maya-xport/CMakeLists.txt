
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

add_definitions(-D_BOOL)

if(WIN32)
    add_definitions(-DNT_PLUGIN)
endif(WIN32)

if(OSX)
    add_definitions(-DMAC_PLUGIN)
endif(OSX)

ac_add_plugin(
    maya-xport y60/maya-xport
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60scene y60modelling
    EXTERNS MAYA
)

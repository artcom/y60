
set(SOURCES
AnimationExporter.cpp
CinemaHelpers.cpp
Main.cpp
MaterialExporter.cpp
SceneExporter.cpp
ShapeExporter.cpp
)

set(HEADERS
AnimationExporter.h
CinemaHelpers.h
Exceptions.h
MaterialExporter.h
SceneExporter.h
ShapeExporter.h
c4d_include.h
)


if(WIN32)
    add_definitions( -D__PC)
endif(WIN32)
if(OSX)
    add_definitions( -D__MAC)
endif(OSX)


add_definitions( -DC4D_API_VERSION=${C4D_VERSION})

ac_add_plugin(
    c4d-xport y60/c4d-xport
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}  
    DEPENDS aslbase aslmath aslzip asldom    
            y60base y60image y60video y60scene y60modelling paintlib
    EXTERNS C4D
)

if(WIN32)
    set_target_properties(c4d-xport PROPERTIES COMPILE_FLAGS "/vmg /vms")
endif(WIN32)

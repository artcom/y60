if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

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


ac_add_executable(
    ac-cinemax
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}  
    DEPENDS aslbase aslmath aslzip asldom    
            y60base y60image y60video y60scene y60modelling paintlib
            #c4dapi c4dcustom c4dgv c4dlibs c4dpreview c4dscaling 
    EXTERNS ${EXTERNALS}
)

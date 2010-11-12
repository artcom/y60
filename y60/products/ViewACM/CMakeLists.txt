set(SOURCES
viewACM.cpp
CompressionType.cpp
MovieScreen.cpp
ScreenRegion.cpp
FileBasedMovie.cpp
)

set(HEADERS
CompressionType.h
MovieScreen.h
MovieBase.h
Image.h
FileBasedMovie.h
ScreenRegion.h
)

list( APPEND EXTERNALS OPENEXR )


if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")    
endif(WIN32)

ac_add_executable(
    viewacm
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase aslmath aslserial aslnet aslipc asldom aslxpath
            y60base y60scene y60image y60video
            y60input y60inet y60glrender y60glutil y60jsbase y60jslib
            y60gltext y60sdlengine ${Y60_SDLMAIN}
            sdlttf2 jsspidermonkey y60modelling aslzip paintlib
    EXTERNS ${EXTERNALS}
    INSTALL_COMPONENT Y60_utilities
)

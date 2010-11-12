set(SOURCES
generateACM.cpp
ACMTextureCompressor.cpp
../ViewACM/CompressionType.cpp
)

set(HEADERS
ACMTextureCompressor.h
)

list( APPEND EXTERNALS OPENEXR )

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
endif(WIN32)

ac_add_executable(
    genacm
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

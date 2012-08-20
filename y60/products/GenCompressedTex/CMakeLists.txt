list( APPEND EXTERNALS OPENEXR )

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

ac_add_executable(
    gencompressedtex
    SOURCES generateCompressedTex.cpp
    DEPENDS aslbase aslmath aslserial aslnet aslipc asldom aslxpath
            y60base y60scene y60image y60video
            y60input y60inet y60glrender y60glutil y60jsbase y60jslib
            y60gltext y60sdlengine ${Y60_SDLMAIN}
            sdlttf2 jsspidermonkey y60modelling aslzip paintlib
    EXTERNS ${EXTERNALS}
    INSTALL_COMPONENT gencompressedtex
)

cpack_add_component( gencompressedtex
            DESCRIPTION "gencompressedtex"
            DISABLED
            GROUP Utilities)
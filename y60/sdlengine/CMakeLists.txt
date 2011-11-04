
set(SOURCES
JSRenderWindow.cpp
SDLApp.cpp
SDLEventSource.cpp
SDLText.cpp
SDLTextRenderer.cpp
SDLWindow.cpp
jssdl.cpp
)

set(HEADERS
JSRenderWindow.h
SDLApp.h
SDLEventSource.h
SDLFontInfo.h
SDLText.h
SDLTextRenderer.h
SDLWindow.h
jssdl.h
y60_sdlengine_settings.h
)

set(EXTERNS SDL GLEW ${JSEXTERN})

if(OSX)
    list(APPEND EXTERNS COCOA AppKit)
    list(APPEND SOURCES SDLWindowOSXUtils.mm)
    list(APPEND HEADERS SDLWindowOSXUtils.h)
endif(OSX)

ac_add_library(
    y60sdlengine y60/sdlengine
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS ${EXTERNS}
    DEPENDS aslbase aslmath aslserial aslnet aslipc asldom aslxpath aslzip
            y60base y60scene y60image y60video y60input y60inet y60glrender
            y60glutil y60jsbase y60jslib y60gltext acsdlttf paintlib ${JSDEPENDS}
)

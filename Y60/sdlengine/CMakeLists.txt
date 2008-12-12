
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
)

ac_add_library(
    y60sdlengine y60/sdlengine
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS SDL OPENEXR
    DEPENDS aslbase aslmath aslserial aslnet aslipc asldom aslxpath
            y60base y60scene y60image y60video y60input y60inet y60glrender y60glutil y60jsbase y60jslib y60gltext
	    sdlttf
    TESTS   ${TESTS}
)


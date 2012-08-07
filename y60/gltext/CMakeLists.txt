
set(SOURCES
BitFont.cpp
BitmapTextRenderer.cpp
Bookman72.cpp
Courier24.cpp
Helv24.cpp
Screen13.cpp
Screen15.cpp
Screen8.cpp
SyntaxBold18.cpp
SyntaxMed144.cpp
Text.cpp
TextStyle.cpp
TextRenderer.cpp
TextRendererManager.cpp
Times12.cpp
Times14.cpp
Times144.cpp
textUtilities.cpp
)

set(HEADERS
BitFont.h
BitmapTextRenderer.h
Bookman72.h
Courier24.h
Helv24.h
Screen13.h
Screen15.h
Screen8.h
SyntaxBold18.h
SyntaxMed144.h
TTFFontInfo.h
TTFTextRenderer.h
Text.h
TextStyle.h
TextRenderer.h
TextRendererManager.h
Times12.h
Times14.h
Times144.h
textUtilities.h
y60_gltext_settings.h
)

ac_add_library(
    y60gltext y60/gltext
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS GLEW
    DEPENDS aslbase aslmath asldom
            y60base y60scene y60image y60video y60glutil paintlib
    TESTS   ${TESTS}
    VERSION ${VERSION_STRING}
    SOVERSION ${VERSION_MAJOR}
    RUNTIME_INSTALL_COMPONENT ${RUNTIME_COMPONENT}
    DEVELOPMENT_INSTALL_COMPONENT ${DEVELOPMENT_COMPONENT}
)


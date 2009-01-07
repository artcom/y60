
set(SOURCES
AbstractRenderWindow.cpp
CharacterMover.cpp
GLContextRegistry.cpp
JSApp.cpp
JSBSpline.cpp
JSCoordSpline.cpp
JSGLResourceManager.cpp
JSKeyframe.cpp
JSLocale.cpp
JSLogger.cpp
JSModellingFunctions.cpp
JSOffscreenRenderArea.cpp
JSPerlinNoise.cpp
JSRenderer.cpp
JSRequest.cpp
JSRequestManager.cpp
JSRequestWrapper.cpp
JSScene.cpp
JSSerial.cpp
JSStringMover.cpp
JSSvgPath.cpp
JSTestServer.cpp
JSZipWriter.cpp
JSfile_functions.cpp
JSintersection_functions.cpp
OffscreenRenderArea.cpp
Timeout.cpp
jscpp.cpp
)

set(HEADERS
AbstractRenderWindow.h
CharacterMover.h
GLContextRegistry.h
IJSExtension.h
IRendererExtension.h
JSAbstractRenderWindow.h
JSApp.h
JSBSpline.h
JSCoordSpline.h
JSGLResourceManager.h
JSKeyframe.h
JSLocale.h
JSLogger.h
JSModellingFunctions.h
JSOffscreenRenderArea.h
JSPerlinNoise.h
JSRenderer.h
JSRequest.h
JSRequestManager.h
JSRequestWrapper.h
JSScene.h
JSSerial.h
JSStringMover.h
JSSvgPath.h
JSTestServer.h
JSZipWriter.h
JSfile_functions.h
JSintersection_functions.h
OffscreenRenderArea.h
Timeout.h
jscpp.h
)

ac_add_library(
    y60jslib y60/jslib
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase aslmath asldom aslxpath aslserial
            y60base y60scene y60image y60video y60input y60inet y60glrender
            y60glutil y60gltext y60jsbase y60modelling paintlib spidermonkey
    EXTERNS OPENEXR GLEW
    TESTS   ${TESTS}
)


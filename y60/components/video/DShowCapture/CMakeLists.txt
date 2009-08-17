
set(SOURCES
DShowCapture.cpp
DShowGraph.cpp
DShowHelper.cpp
DXSampleGrabber.cpp
)

set(HEADERS
DShowHelper.h
DShowCapture.h
DShowGraph.h
DXSampleGrabber.h
y60_dshowcapture_settings.h
)

set(EXTERNALS FFMPEG PTHREADVC DIRECTX WMSDK ATL WINDOWSSDK)

y60_add_component(
    DShowCapture
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib y60video
    EXTERNS ${EXTERNALS}
)

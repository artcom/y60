
set(SOURCES
GStreamerCapturePlugin.cpp
)

set(HEADERS
GStreamerCapturePlugin.h
)

y60_add_component(
    GStreamerCapture
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
)


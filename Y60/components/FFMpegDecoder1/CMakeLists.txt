
set(SOURCES
FFMpegDecoder1.cpp
)

set(HEADERS
FFMpegDecoder1.h
)

y60_add_component(
    FFMpegDecoder1
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS OPENEXR SPIDERMONKEY
)


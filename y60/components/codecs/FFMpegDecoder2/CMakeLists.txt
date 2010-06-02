
set(SOURCES
Demux.cpp
FFMpegDecoder2.cpp
FFMpegURLProtocol.cpp
)

set(HEADERS
Demux.h
FFMpegDecoder2.h
FFMpegURLProtocol.h
y60_ffmpegdecoder2_settings.h
)

list(APPEND EXTERNALS PAINTLIB FFMPEG)

add_definitions(-D__STDC_CONSTANT_MACROS) # XXX: why? what? aeh!?

y60_add_component(
    FFMpegDecoder2
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS asldom aslaudio y60video
    EXTERNS ${EXTERNALS}
)

# Disabled due to excessive flakyness ... see y60/video/testLeak.js
#y60_add_jstest(Leak mod_ffmpeg2)
y60_add_jstest(Movie mod_ffmpeg2)
y60_add_jstest(Seek mod_ffmpeg2)

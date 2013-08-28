
set(SOURCES
Demux.cpp
FFMpegDecoder2.cpp
)

set(HEADERS
Demux.h
FFMpegDecoder2.h
y60_ffmpegdecoder2_settings.h
)

list(APPEND EXTERNALS PAINTLIB FFMPEG)

add_definitions(-D__STDC_CONSTANT_MACROS) # ffmpeg is c99 only and needs some c macros

y60_add_component(
    FFMpegDecoder2
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS asldom aslaudio y60video y60base
    EXTERNS ${EXTERNALS}
)

# Disabled due to excessive flakyness ... see y60/video/testLeak.js
#y60_add_jstest(Leak mod_ffmpeg2)
y60_add_jstest(Movie mod_ffmpeg2)
y60_add_jstest(Seek mod_ffmpeg2)

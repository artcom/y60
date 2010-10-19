
set(SOURCES
FFMpegDecoder1.cpp
)

set(HEADERS
FFMpegDecoder1.h
y60_ffmpegdecoder1_settings.h
)

list(APPEND EXTERNALS FFMPEG)

add_definitions(-D__STDC_CONSTANT_MACROS) # ffmpeg is c99 only and needs some c macros

y60_add_component(
    FFMpegDecoder1
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60video
    EXTERNS ${EXTERNALS}
)

y60_add_jstest(Leak mod_ffmpeg1)

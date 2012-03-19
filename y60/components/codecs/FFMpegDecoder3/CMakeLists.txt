
set(SOURCES
AsyncDemuxer.cpp
FFMpegDecoder3.cpp
)

set(HEADERS
PacketQueue.h
AsyncDemuxer.h
FFMpegDecoder3.h
y60_ffmpegdecoder3_settings.h
)

if(NOT WIN32)
    find_package(Boost COMPONENTS thread)
endif(NOT WIN32)

list(APPEND EXTERNALS Boost PAINTLIB FFMPEG)

add_definitions(-D__STDC_CONSTANT_MACROS) # ffmpeg is c99 only and needs some c macros

y60_add_component(
    FFMpegDecoder3
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS asldom aslaudio y60video
    EXTERNS ${EXTERNALS}
)

# Disabled due to excessive flakyness ... see y60/video/testLeak.js
#y60_add_jstest(Leak mod_ffmpeg3)
#y60_add_jstest(Movie mod_ffmpeg3)
#y60_add_jstest(Seek mod_ffmpeg3)

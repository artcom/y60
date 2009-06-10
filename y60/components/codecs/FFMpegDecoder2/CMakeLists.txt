
set(SOURCES
Demux.cpp
FFMpegDecoder2.cpp
FFMpegURLProtocol.cpp
VideoMsg.cpp
VideoMsgQueue.cpp
)

set(HEADERS
Demux.h
FFMpegDecoder2.h
FFMpegURLProtocol.h
VideoMsg.h
VideoMsgQueue.h
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

y60_add_jstest(Leak mod_ffmpeg2)

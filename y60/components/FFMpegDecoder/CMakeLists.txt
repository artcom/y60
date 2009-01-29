
set(SOURCES
DecoderContext.cpp
FFMpegDecoder.cpp
FrameAnalyser.cpp
FrameConveyor.cpp
)

set(HEADERS
AudioFrame.h
DecoderContext.h
FFMpegDecoder.h
FFMpegPixelEncoding.h
FrameAnalyser.h
FrameConveyor.h
VideoFrame.h
y60_ffmpegdecoder_settings.h
)

list( APPEND EXTERNALS AVFORMAT AVCODEC SWSCALE PAINTLIB OPENEXR )

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

add_definitions(-D__STDC_CONSTANT_MACROS)

y60_add_component(
    FFMpegDecoder
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslaudio
            y60video 
    EXTERNS ${EXTERNALS}
)


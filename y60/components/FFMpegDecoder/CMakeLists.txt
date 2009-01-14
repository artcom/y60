
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

list( APPEND EXTERNALS AVFORMAT AVCODEC PAINTLIB OPENEXR )

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

y60_add_component(
    FFMpegDecoder
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslaudio
            y60video 
    EXTERNS ${EXTERNALS}
)


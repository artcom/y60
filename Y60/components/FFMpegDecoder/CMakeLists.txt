
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
)

y60_add_component(
    FFMpegDecoder
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS OPENEXR
)



set(SOURCES
Capture.cpp
M60Decoder.cpp
Movie.cpp
MovieDecoderBase.cpp
VideoMsg.cpp
VideoMsgQueue.cpp

)

set(HEADERS
AsyncDecoder.h
Capture.h
CaptureDevice.h
M60Decoder.h
M60Header.h
Movie.h
MovieDecoderBase.h
MovieEncoding.h
VideoMsg.h
VideoMsgQueue.h
y60_video_settings.h
)

ac_add_library(
    y60video y60/video
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase asldom aslmath aslzip y60base y60image paintlib
    TESTS   ${TESTS}
)

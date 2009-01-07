
set(SOURCES
Capture.cpp
M60Decoder.cpp
Movie.cpp
MovieDecoderBase.cpp
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
)

set(TESTS
)

if(OSX)
endif(OSX)

if(LINUX)
endif(LINUX)

if(WIN32)
endif(WIN32)

ac_add_library(
    y60video y60/video
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase asldom aslmath aslzip y60base y60image paintlib
    EXTERNS OPENEXR
    TESTS   ${TESTS}
)


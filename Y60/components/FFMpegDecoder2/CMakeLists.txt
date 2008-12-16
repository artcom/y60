
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
)

y60_add_component(
    FFMpegDecoder2
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS OPENEXR SPIDERMONKEY
)


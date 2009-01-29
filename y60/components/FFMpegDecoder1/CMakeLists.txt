
set(SOURCES
FFMpegDecoder1.cpp
)

set(HEADERS
FFMpegDecoder1.h
y60_ffmpegdecoder1_settings.h
)

list( APPEND EXTERNALS FFMPEG PAINTLIB OPENEXR )

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

add_definitions(-D__STDC_CONSTANT_MACROS)

y60_add_component(
    FFMpegDecoder1
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60video
    EXTERNS ${EXTERNALS}
)


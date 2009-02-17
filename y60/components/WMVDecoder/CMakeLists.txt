
set(SOURCES
WMVDecoder.cpp
)

set(HEADERS
WMVDecoder.h
y60_wmvdecoder_settings.h
)

list( APPEND EXTERNALS OPENEXR FFMPEG)
if(WIN32)
    set(WMVCORE_LIBRARIES wmvcore) # XXX: write find package
    list( APPEND EXTERNALS PTHREADVC WMVCORE )
endif(WIN32)

y60_add_component(
    WMVDecoder
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslaudio y60video
    EXTERNS ${EXTERNALS}
)




set(SOURCES
WMVDecoder.cpp
)

set(HEADERS
WMVDecoder.h
y60_wmvdecoder_settings.h
)

set(EXTERNALS FFMPEG PTHREADVC WMSDK ATL)

add_definitions(-D__STDC_CONSTANT_MACROS) # ffmpeg is c99 only and needs some c macros

y60_add_component(
    WMVDecoder
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslaudio y60video
    EXTERNS ${EXTERNALS}
)

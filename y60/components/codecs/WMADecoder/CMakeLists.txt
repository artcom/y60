
set(SOURCES
WMADecoder.cpp
WMADecoderFactory.cpp
WMHelper.cpp
)

set(HEADERS
WMADecoder.h
WMADecoderFactory.h
WMHelper.h
y60_wmadecoder_settings.h
)

set(EXTERNALS FFMPEG PTHREADVC DIRECTX WMSDK ATL)

add_definitions(-D__STDC_CONSTANT_MACROS) # ffmpeg is c99 only and needs some c macros

y60_add_component(
    WMADecoder
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60sound 
    EXTERNS ${EXTERNALS}
)

# XXX: why?
#y60_add_jstest(localtest mod_WMADecoder)

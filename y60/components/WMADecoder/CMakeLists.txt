
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

y60_add_component(
    y60WMADecoder
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60sound 
    EXTERNS ${EXTERNALS}
)

#y60_add_jstest(localtest mod_WMADecoder)

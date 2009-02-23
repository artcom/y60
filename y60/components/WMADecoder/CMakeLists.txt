
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

list( APPEND EXTERNALS FFMPEG)
if(WIN32)
    list( APPEND EXTERNALS PTHREADVC DIRECTX WMSDK )
endif(WIN32)

y60_add_component(
    WMADecoder
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60sound 
    EXTERNS ${EXTERNALS}
)

y60_add_jstest(localtest mod_WMADecoder)


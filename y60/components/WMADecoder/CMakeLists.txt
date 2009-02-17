
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
    set(WMVCORE_LIBRARIES wmvcore) # XXX: write find package
    list( APPEND EXTERNALS PTHREADVC DIRECTX WMVCORE )
endif(WIN32)

y60_add_component(
    WMADecoder
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60sound 
    EXTERNS ${EXTERNALS}
)

y60_add_jstest(localtest WMADecoder)



set(SOURCES
OpenEXRDecoder.cpp
OpenEXRDecoderPlugin.cpp
)

set(HEADERS
OpenEXRDecoder.h
PLDataSourceStreamAdapter.h
y60_openexrdecoder_settings.h
)

list(APPEND EXTERNS OPENEXR)
if(WIN32)
    list(APPEND EXTERNS PTHREADVC)
endif(WIN32)

y60_add_component(
    OpenEXR
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS ${EXTERNS} 
    DEPENDS paintlib
)


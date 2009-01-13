
set(SOURCES
OpenEXRDecoder.cpp
OpenEXRDecoderPlugin.cpp
)

set(HEADERS
OpenEXRDecoder.h
PLDataSourceStreamAdapter.h
)

list(APPEND EXTERNS OPENEXR)
if(WIN32)
    list(APPEND EXTERNS PTHREADVC)
endif(WIN32)

y60_add_component(
    OpenEXRDecoder
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS ${EXTERNS} 
    DEPENDS paintlib
)


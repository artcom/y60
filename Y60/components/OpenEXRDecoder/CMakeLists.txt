
set(SOURCES
OpenEXRDecoder.cpp
OpenEXRDecoderPlugin.cpp
)

set(HEADERS
OpenEXRDecoder.h
PLDataSourceStreamAdapter.h
)

y60_add_component(
    OpenEXRDecoder
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS OPENEXR
)



set(SOURCES
OpenEXRDecoder.cpp
OpenEXRDecoderPlugin.cpp
)

set(HEADERS
OpenEXRDecoder.h
PLDataSourceStreamAdapter.h
y60_openexrdecoder_settings.h
)

y60_add_component(
    OpenEXR
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS OPENEXR
)

y60_add_jstest(OpenEXR)

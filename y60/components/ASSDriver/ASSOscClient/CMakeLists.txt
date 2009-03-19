
set(SOURCES
ASSOscClient.cpp
ASSOscPlugin.cpp
JSASSOscClient.cpp
)

set(HEADERS
ASSOscClient.h
JSASSOscClient.h
y60_oscclient_settings.h
)

y60_add_component(
    y60ASSOscClient
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib y60ASSCore oscpackip oscpackosc
    RUNTIME_INSTALL_COMPONENT ASS_runtime
    DEVELOPMENT_INSTALL_COMPONENT ASS_development
)

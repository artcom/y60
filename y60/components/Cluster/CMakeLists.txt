
set(SOURCES
ClusterExtension.cpp
)

set(HEADERS
y60_clusterextension_settings.h
)

y60_add_component(
    Cluster
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS SPIDERMONKEY OPENEXR GLEW
)


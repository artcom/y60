
set(SOURCES
ClusterExtension.cpp
)

set(HEADERS
)

y60_add_component(
    ClusterExtension
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS SPIDERMONKEY OPENEXR GLEW
)


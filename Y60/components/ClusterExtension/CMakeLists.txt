
set(SOURCES
ClusterExtension.cpp
)

set(HEADERS
)

y60_add_component(
    ClusterExtension
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS OPENEXR SPIDERMONKEY
)



set(SOURCES
AssetRequest.cpp
CMSCache.cpp
CMSCachePlugIn.cpp
JSCMSCache.cpp
RequestThread.cpp
)

set(HEADERS
AssetRequest.h
CMSCache.h
JSCMSCache.h
RequestThread.h
)

y60_add_component(
    CMSCache
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS OPENEXR
)


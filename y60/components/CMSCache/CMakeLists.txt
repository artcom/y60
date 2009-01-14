
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

list( APPEND EXTERNALS SPIDERMONKEY OPENEXR )

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

y60_add_component(
    CMSCache
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS ${EXTERNALS}
)



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

list( APPEND EXTERNALS OPENEXR )   

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

y60_add_component(
    y60ASSOscPlugin 
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS ${EXTERNALS}
)


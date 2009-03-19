list( APPEND EXTERNALS OPENEXR )

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

ac_add_executable(
    y60-gen-cubemap
    SOURCES generateCubeMap.cpp
    DEPENDS aslbase aslmath aslzip asldom y60base y60image paintlib
    EXTERNS ${EXTERNALS}
    INSTALL_COMPONENT Y60_utilities
)

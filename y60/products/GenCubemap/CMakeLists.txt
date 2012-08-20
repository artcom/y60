list( APPEND EXTERNALS OPENEXR )

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

ac_add_executable(
    gencubemap
    SOURCES generateCubeMap.cpp
    DEPENDS aslbase aslmath aslzip asldom y60base y60image paintlib
    EXTERNS ${EXTERNALS}
    INSTALL_COMPONENT gencubemap
)

cpack_add_component( gencubemap
            DESCRIPTION "gencubemap"
            DISABLED
            GROUP Utilities)
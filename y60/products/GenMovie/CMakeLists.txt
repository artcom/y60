list( APPEND EXTERNALS OPENEXR )

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

ac_add_executable(
    genmovie
    SOURCES generateMovie.cpp
    DEPENDS 
        aslbase aslmath aslzip asldom y60base y60image paintlib
    EXTERNS ${EXTERNALS}
    INSTALL_COMPONENT genmovie
)

cpack_add_component( genmovie
            DESCRIPTION "genmovie"
            DISABLED
            GROUP Utilities)
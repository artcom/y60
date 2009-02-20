list( APPEND EXTERNALS OPENEXR )

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

ac_add_executable(
    y60-gen-movie
    SOURCES
       generateMovie.cpp
    DEPENDS 
        aslbase aslmath aslzip asldom y60base y60image paintlib
    EXTERNS ${EXTERNALS}
)

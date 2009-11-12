if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

ac_add_executable(
    greplace
    SOURCES
       greplace.cpp
    DEPENDS aslbase       
    EXTERNS ${EXTERNALS}
)

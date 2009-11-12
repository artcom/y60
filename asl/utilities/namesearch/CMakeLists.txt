if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

ac_add_executable(
    namesearch
    SOURCES
       namesearch.cpp
    DEPENDS aslbase
    EXTERNS ${EXTERNALS}
)

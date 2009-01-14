
set(SOURCES
JSproc_functions.cpp
ProcFunctionsPlugIn.cpp
)

set(HEADERS
JSproc_functions.h
)

list( APPEND EXTERNALS SPIDERMONKEY OPENEXR NVIDIA_CG)

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

y60_add_component(
    ProcFunctions
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS ${EXTERNALS}
)

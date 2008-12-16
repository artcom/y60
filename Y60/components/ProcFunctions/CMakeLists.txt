
set(SOURCES
JSproc_functions.cpp
ProcFunctionsPlugIn.cpp
)

set(HEADERS
JSproc_functions.h
)

y60_add_component(
    ProcFunctions
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS OPENEXR SPIDERMONKEY
)

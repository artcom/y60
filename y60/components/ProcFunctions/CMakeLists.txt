
set(SOURCES
JSproc_functions.cpp
ProcFunctionsPlugIn.cpp
)

set(HEADERS
JSproc_functions.h
y60_procfunctions_settings.h
)

y60_add_component(
    ProcFunctions
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
)


y60_add_jstest(ProcFunctions ProcFunctions)
y60_add_jstest(ImageLeak ProcFunctions)
y60_add_jstest(SceneLeakage ProcFunctions)

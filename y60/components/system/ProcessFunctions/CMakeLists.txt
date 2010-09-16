
set(SOURCES
JSproc_functions.cpp
ProcFunctionsPlugIn.cpp
)

set(HEADERS
JSproc_functions.h
y60_procfunctions_settings.h
)

y60_add_component(
    ProcessFunctions
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
)

y60_add_jstest(ProcFunctions mod_ProcFunctions)
# y60_add_jstest(ImageLeak mod_ProcFunctions) disbaled flaky test.
y60_add_jstest(SceneLeakage mod_ProcFunctions)


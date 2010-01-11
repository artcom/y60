
set(SOURCES
JShashing_functions.cpp
HashingFunctionsPlugIn.cpp
)

set(HEADERS
JShashing_functions.h
y60_hashingfunctions_settings.h
)

y60_add_component(
    HashingFunctions
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS CRYPTOPP
)


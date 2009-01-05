
set(SOURCES
Request.cpp
RequestManager.cpp
TestServer.cpp
)

set(HEADERS
Request.h
RequestManager.h
TestServer.h
)

set(TESTS
Request
)

if(OSX)
endif(OSX)

if(LINUX)
endif(LINUX)

if(WIN32)
endif(WIN32)

ac_add_library(
    y60inet y60/inet
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase aslnet aslipc
    EXTERNS CURL
    TESTS   ${TESTS}
)


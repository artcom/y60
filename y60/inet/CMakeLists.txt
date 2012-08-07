
set(SOURCES
Request.cpp
RequestManager.cpp
TestServer.cpp
)

set(HEADERS
Request.h
RequestManager.h
TestServer.h
y60_inet_settings.h
)

set(TESTS
Request
)

ac_add_library(
    y60inet y60/inet
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase aslnet aslipc
    EXTERNS CURL
    TESTS   ${TESTS}
    VERSION ${VERSION_STRING}
    SOVERSION ${VERSION_MAJOR}
    RUNTIME_INSTALL_COMPONENT ${RUNTIME_COMPONENT}
    DEVELOPMENT_INSTALL_COMPONENT ${DEVELOPMENT_COMPONENT}
)


cmake_minimum_required(VERSION 2.6.2)

project(FakeATL)

include(AcMake)

list(INSERT CMAKE_MODULE_PATH 0 ${FakeATL_SOURCE_DIR})

ac_add_project(
    FakeATL
    CUSTOM_SCRIPTS
        FindFakeATLIncludes.cmake
)

ac_add_library(
    fakeatl fakeatl
    SOURCES dummy.cpp
    HEADERS atlbase.h atlcom.h atlcomcli.h atlconv.h atlctl.h atlhost.h
    RUNTIME_INSTALL_COMPONENT ${RUNTIME_COMPONENT}
    DEVELOPMENT_INSTALL_COMPONENT ${DEVELOPMENT_COMPONENT}
)

ac_end_project(FakeATL)

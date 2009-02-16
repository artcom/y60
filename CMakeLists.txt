
enable_testing()

cmake_minimum_required(VERSION 2.6)

project(PRO60)

# make build-integrated acmake available
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/acmake/modules)

# make build-integrated packages available
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_BINARY_DIR}/asl)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_BINARY_DIR}/y60)

# set some flags telling sub-packages that
# they are being built as a subpackage
set(ACMAKE_INTEGRATED_BUILD YES)
set(ACSDLTTF_INTEGRATED_BUILD YES)
set(PAINTLIB_INTEGRATED_BUILD YES)
set(SPIDERMONKEY_INTEGRATED_BUILD YES)
set(ASL_INTEGRATED_BUILD YES)
set(Y60_INTEGRATED_BUILD YES)

# look for ART+COM companion libraries
find_package( ACCL )
if(ACCL_FOUND)
    accl_register_searchpath()
endif(ACCL_FOUND)

set(ACMAKE_BUILDINFO_TEMPLATE
    ${CMAKE_CURRENT_SOURCE_DIR}/asl/base/buildinfo.cpp.in)
# add paths to our internal staged includes
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/netsrc/paintlib/include
    ${CMAKE_CURRENT_BINARY_DIR}/netsrc/SDL_ttf2/include
)

# traverse our subdirectories
add_subdirectory(acmake)
add_subdirectory(netsrc)
add_subdirectory(asl)
add_subdirectory(y60)


# XXX hack for testing the scenetests and tutorials
if(EXISTS ${CMAKE_SOURCE_DIR}/../testmodels)
  add_subdirectory(../testmodels ${CMAKE_CURRENT_BINARY_DIR}/testmodels)
endif(EXISTS ${CMAKE_SOURCE_DIR}/../testmodels)
if(EXISTS ${CMAKE_SOURCE_DIR}/../tutorials)
  add_subdirectory(../tutorials ${CMAKE_CURRENT_BINARY_DIR}/tutorials)
endif(EXISTS ${CMAKE_SOURCE_DIR}/../tutorials)


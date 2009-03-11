
enable_testing()

cmake_minimum_required(VERSION 2.6.2)

project(PRO60)

# make build-integrated acmake available
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/acmake/modules)

# make build-integrated packages available
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/asl)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_BINARY_DIR}/asl)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/y60)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_BINARY_DIR}/y60)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_BINARY_DIR}/netsrc/SDL_ttf2)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_BINARY_DIR}/netsrc/paintlib)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_BINARY_DIR}/netsrc/oscpack)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_BINARY_DIR}/netsrc/fakeatl)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/netsrc/fakeatl) # XXX: [IA] this should not be needed
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_BINARY_DIR}/netsrc/spidermonkey) # XXX: tracemonkey?

# this replaces the (currently) broken transient depend include path propagation
include_directories(
    ${CMAKE_BINARY_DIR}/netsrc/paintlib/include
    ${CMAKE_BINARY_DIR}/netsrc/spidermonkey/include # XXX: tracemonkey
    ${CMAKE_BINARY_DIR}/netsrc/SDL_ttf2/include
    ${CMAKE_BINARY_DIR}/netsrc/oscpack
    ${CMAKE_SOURCE_DIR}/netsrc/fakeatl
)

# tell acmake that it is being used in integrated mode
set(ACMAKE_INTEGRATED_BUILD YES)

# set the global default buildinfo template
set(ACMAKE_BUILDINFO_TEMPLATE
    ${CMAKE_CURRENT_SOURCE_DIR}/asl/base/buildinfo.cpp.in)

# add and load acmake
add_subdirectory(acmake)
include(AcMake)

# add our packages
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

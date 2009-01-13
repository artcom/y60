
enable_testing()

cmake_minimum_required(VERSION 2.6)

project(PRO60)

# make build-integrated acmake available
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/acmake/modules)

if(WIN32)
    list( INSERT CMAKE_PREFIX_PATH 0 "$ENV{PRO60_DEPS}")
endif(WIN32)

# set some flags telling sub-packages that
# they are being built as a subpackage
set(ACMAKE_INTEGRATED_BUILD YES)
set(ACSDLTTF_INTEGRATED_BUILD YES)
set(PAINTLIB_INTEGRATED_BUILD YES)
set(SPIDERMONKEY_INTEGRATED_BUILD YES)
set(ASL_INTEGRATED_BUILD YES)
set(Y60_INTEGRATED_BUILD YES)

# add paths to our internal staged includes
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/netsrc/spidermonkey/include
    ${CMAKE_CURRENT_BINARY_DIR}/netsrc/paintlib/include
    ${CMAKE_CURRENT_BINARY_DIR}/netsrc/SDL_ttf2/include
)

# traverse our subdirectories
add_subdirectory(acmake)
add_subdirectory(netsrc)
add_subdirectory(asl)
add_subdirectory(y60)

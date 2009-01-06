
enable_testing()

cmake_minimum_required(VERSION 2.6)

project(PRO60)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/acmake/modules)

set(ACMAKE_INTEGRATED_BUILD YES)
set(ACSDLTTF_INTEGRATED_BUILD YES)
set(PAINTLIB_INTEGRATED_BUILD YES)
set(SPIDERMONKEY_INTEGRATED_BUILD YES)
set(ASL_INTEGRATED_BUILD YES)
set(Y60_INTEGRATED_BUILD YES)

add_subdirectory(acmake)
add_subdirectory(netsrc)
add_subdirectory(asl)
add_subdirectory(y60)



enable_testing()

cmake_minimum_required(VERSION 2.6)

project(PRO60)

set(ASL_INTEGRATED_BUILD YES)
set(Y60_INTEGRATED_BUILD YES)

add_subdirectory(netsrc)
add_subdirectory(asl)
add_subdirectory(y60)


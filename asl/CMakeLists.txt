#============================================================================
# Copyright (C) 2008, ART+COM Technologies GmbH Berlin
#
# These coded instructions, statements, and computer programs contain
# unpublished proprietary information of ART+COM AG Berlin, and
# are copy protected by law. They may not be disclosed to third parties
# or copied or duplicated in any form, in whole or in part, without the
# specific, prior written permission of ART+COM AG Berlin.
#============================================================================

cmake_minimum_required(VERSION 2.6)

project(ASL)

set(CMAKE_MODULE_PATH ${ASL_SOURCE_DIR} $ENV{ACMAKE}/modules $ENV{ACMAKE_PATH})

include(AcMake)

include(ASLDefinitions)
include(ASLDependencies)

include_directories(../)

add_subdirectory(base)
add_subdirectory(math)
add_subdirectory(audio)
add_subdirectory(raster)
add_subdirectory(dom)
add_subdirectory(xpath)
add_subdirectory(serial)
add_subdirectory(net)
add_subdirectory(ipc)
add_subdirectory(zip)

if(NOT ASL_INTEGRATED_BUILD)
configure_file(
    ${ASL_SOURCE_DIR}/FindASL.cmake.in
    ${ASL_BINARY_DIR}/FindASL.cmake
    @ONLY
)

install(
    FILES ASLDependencies.cmake ASLDefinitions.cmake
    DESTINATION lib/cmake
)

install(
    FILES ${ASL_BINARY_DIR}/FindASL.cmake
    DESTINATION lib/cmake
)

install(
    EXPORT ASL
    DESTINATION lib/cmake
    FILE ASLExport.cmake
)
endif(NOT ASL_INTEGRATED_BUILD)


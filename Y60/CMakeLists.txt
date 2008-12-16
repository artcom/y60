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

project(Y60)

set(CMAKE_MODULE_PATH ${Y60_SOURCE_DIR} $ENV{ACMAKE}/modules $ENV{ACMAKE_PATH})

include(AcMake)

include(Y60Definitions)
include(Y60Dependencies)

# declarations

include_directories(../)

macro(y60_add_component COMPONENT_NAME)
    ac_add_plugin(
        ${COMPONENT_NAME} y60/components
        ${ARGN}
    )
endmacro(y60_add_component)


add_subdirectory(base)
add_subdirectory(sound)
add_subdirectory(image)
add_subdirectory(video)
add_subdirectory(scene)
add_subdirectory(glutil)
add_subdirectory(gltext)
add_subdirectory(glrender)
add_subdirectory(input)
add_subdirectory(inet)
add_subdirectory(modelling)
add_subdirectory(jsbase)
add_subdirectory(jslib)
add_subdirectory(sdlengine)

add_subdirectory(products)
add_subdirectory(components)

if(NOT Y60_INTEGRATED_BUILD)
configure_file(
    ${Y60_SOURCE_DIR}/FindY60.cmake.in
    ${Y60_BINARY_DIR}/FindY60.cmake
    @ONLY
)

install(
    FILES Y60Dependencies.cmake Y60Definitions.cmake
    DESTINATION lib/cmake
)

install(
    FILES ${Y60_BINARY_DIR}/FindY60.cmake
    DESTINATION lib/cmake
)

install(
    EXPORT Y60
    DESTINATION lib/cmake
    FILE Y60Export.cmake
)
endif(NOT Y60_INTEGRATED_BUILD)

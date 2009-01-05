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

list(APPEND CMAKE_MODULE_PATH ${Y60_SOURCE_DIR})

if(NOT "XX$ENV{ACMAKE}XX" STREQUAL "XXXX")
    list(APPEND CMAKE_MODULE_PATH $ENV{ACMAKE}/modules)
endif(NOT "XX$ENV{ACMAKE}XX" STREQUAL "XXXX")

include(AcMake)

include(Y60Definitions)
include(Y60Dependencies)
include(Y60Utilities)

# declarations

include_directories(../)

# engine libraries
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

# libraries depending on gtk
if(GTKMM_FOUND AND GTKGLEXT_FOUND AND GLADEMM_FOUND)
    add_subdirectory(acgtk)
    add_subdirectory(jsgtk)
endif(GTKMM_FOUND AND GTKGLEXT_FOUND AND GLADEMM_FOUND)

# executables and plugins
add_subdirectory(products)
add_subdirectory(components)

# install script library
install(
    DIRECTORY js
    DESTINATION lib/y60
    FILES_MATCHING
        PATTERN "*.js"
        PATTERN ".svn" EXCLUDE
)

# install shader library
install(
    DIRECTORY shader
    DESTINATION lib/y60
    FILES_MATCHING
        PATTERN "*.cg"
        PATTERN "*.xml"
        PATTERN "*.png"
        PATTERN "*.rgb"
        PATTERN ".svn" EXCLUDE
)

# export cmake package
if(NOT Y60_INTEGRATED_BUILD)
configure_file(
    ${Y60_SOURCE_DIR}/FindY60.cmake.in
    ${Y60_BINARY_DIR}/FindY60.cmake
    @ONLY
)

install(
    FILES Y60Dependencies.cmake Y60Definitions.cmake Y60Utilities.cmake
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


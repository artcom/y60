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

project(y60)

set(CMAKE_MODULE_PATH ../acmake/modules ${CMAKE_INSTALL_PREFIX}/lib/cmake ${CMAKE_MODULE_PATH})

include(AcMake)
include(FindPkgConfig)

# our own dependencies
include(asl)
include(paintlib)
include(spidermonkey)
include(sdlttf)

# external dependencies
find_package(GLEW)
find_package(Cg)

# paintlib transitive
find_package(PNG)
find_package(GIF)
find_package(TIFF)
find_package(JPEG)

pkg_search_module(OPENEXR REQUIRED OpenEXR)
pkg_search_module(GLIB REQUIRED glib-2.0)
pkg_search_module(CURL REQUIRED libcurl)
pkg_search_module(AVCODEC  REQUIRED libavcodec)
pkg_search_module(AVFORMAT REQUIRED libavformat)

include_directories(../)
include_directories(${CMAKE_INSTALL_PREFIX}/include)

add_definitions(-DAC_BUILT_WITH_CMAKE)

macro(y60_add_component COMPONENT_NAME)
    ac_add_plugin(
        ${COMPONENT_NAME} y60/components/${COMPONENT_NAME}
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

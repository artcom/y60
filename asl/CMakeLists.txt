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

include(AcMake)


# dependencies

include(FindPkgConfig)

#find_package(ZLIB)

pkg_search_module(GLIB2 REQUIRED glib-2.0)
pkg_search_module(CURL  REQUIRED libcurl)

if(LINUX)
  pkg_search_module(ALSA REQUIRED alsa)
endif(LINUX)


# declarations

project(asl)

include_directories(../)

add_subdirectory(base)
add_subdirectory(math)
add_subdirectory(audio)
add_subdirectory(raster)
add_subdirectory(dom)
add_subdirectory(xpath)
add_subdirectory(serial)
add_subdirectory(ipc)
add_subdirectory(net)
#add_subdirectory(zip)



include(FindPkgConfig)

find_package(ZLIB)

pkg_search_module(GLIB2    REQUIRED glib-2.0)
pkg_search_module(CURL     REQUIRED libcurl)
pkg_search_module(OPENEXR  REQUIRED OpenEXR)

if(LINUX)
    pkg_search_module(ALSA REQUIRED alsa)
endif(LINUX)

if(OSX)
    find_library(CARBON     Carbon)
    find_library(AUDIO_UNIT AudioUnit)
endif(OSX)


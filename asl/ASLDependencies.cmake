
include(FindPkgConfig)

find_package(ZLIB)

pkg_search_module(GLIB2    REQUIRED glib-2.0)
find_package(CURL)
find_package(OpenEXR)

if(LINUX)
    pkg_search_module(ALSA REQUIRED alsa)
endif(LINUX)

if(WIN32)
    find_package( PTHREADVC )
endif(WIN32)

if(OSX)
    find_library(CARBON     Carbon)
    find_library(AUDIO_UNIT AudioUnit)
endif(OSX)


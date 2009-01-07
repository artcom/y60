
include(FindPkgConfig)

find_package(ZLIB)


find_package(GLIB2)
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


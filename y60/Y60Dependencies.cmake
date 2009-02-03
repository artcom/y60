# This file resolves the dependencies of Y60

include(AcPlatform)

include(FindPkgConfig)

find_package(ASL)

# exported dependencies
if(NOT PAINTLIB_INTEGRATED_BUILD)
    find_package(Paintlib)
endif(NOT PAINTLIB_INTEGRATED_BUILD)
# if(NOT SPIDERMONKEY_INTEGRATED_BUILD)
#    find_package(Spidermonkey)
# endif(NOT SPIDERMONKEY_INTEGRATED_BUILD)
if(NOT ACSDLTTF_INTEGRATED_BUILD)
    find_package(ACSDLTTF)
endif(NOT ACSDLTTF_INTEGRATED_BUILD)

# external dependencies
find_package(SDL)
find_package(GLEW)
find_package(NvidiaCg)
find_package(OpenEXR)

find_package(GLIB2)
find_package(MOZJS)

pkg_search_module(GTKMM gtkmm-2.4)
pkg_search_module(GLADEMM libglademm-2.4)
pkg_search_module(GTKGLEXT gtkglext-1.0)

if(WIN32)
    find_package(PthreadVC)
    pkg_search_module(CAIRO REQUIRED cairo-win32)
elseif(LINUX)
    pkg_search_module(CAIRO REQUIRED cairo-xlib)
elseif(UNIX AND APPLE)
    #pkg_search_module(CAIRO REQUIRED cairo-xlib)
    pkg_search_module(CAIRO REQUIRED cairo-quartz)
endif(WIN32)
find_package( FFMPEG )
#find_package( Avcodec )
#find_package( Avformat )
#find_package( Swscale )

if( OSX )
    find_library( COCOA_LIBRARIES NAMES Cocoa)
endif( OSX )


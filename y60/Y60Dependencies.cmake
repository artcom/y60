# This file resolves the dependencies of Y60

include(AcPlatform)

include(FindPkgConfig)

# integrated dependencies
if(NOT ASL_INTEGRATED_BUILD)
    find_package(ASL)
endif(NOT ASL_INTEGRATED_BUILD)

# exported dependencies
if(NOT PAINTLIB_INTEGRATED_BUILD)
    find_package(Paintlib)
endif(NOT PAINTLIB_INTEGRATED_BUILD)
if(NOT SPIDERMONKEY_INTEGRATED_BUILD)
    find_package(Spidermonkey)
endif(NOT SPIDERMONKEY_INTEGRATED_BUILD)
if(NOT ACSDLTTF_INTEGRATED_BUILD)
    find_package(ACSDLTTF)
endif(NOT ACSDLTTF_INTEGRATED_BUILD)

# external dependencies
find_package(SDL)
find_package(GLEW)
find_package(Cg)
find_package(OpenEXR)

find_package(GLIB2)
#pkg_search_module(GLIB REQUIRED glib-2.0)

pkg_search_module(GTKMM OPTIONAL gtkmm-2.4)
pkg_search_module(GLADEMM OPTIONAL libglademm-2.4)
pkg_search_module(GTKGLEXT OPTIONAL gtkglext-1.0)

if(WIN32)
    pkg_search_module(CAIRO REQUIRED cairo-win32)
else(WIN32)
    pkg_search_module(CAIRO REQUIRED cairo-xlib)
endif(WIN32)

# ffmpeg
find_package(Ffmpeg)


if( OSX )
    find_library( COCOA_LIBRARIES NAMES Cocoa)
endif( OSX )


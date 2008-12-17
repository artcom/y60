
include(FindPkgConfig)

# integrated dependencies
if(NOT ASL_INTEGRATED_BUILD)
    find_package(ASL)
endif(NOT ASL_INTEGRATED_BUILD)

# exported dependencies
find_package(Paintlib)
find_package(Spidermonkey)
find_package(SDLTTF)

# external dependencies
find_package(GLEW)
find_package(Cg)

pkg_search_module(GLIB REQUIRED glib-2.0)

# ffmpeg
pkg_search_module(AVCODEC  REQUIRED libavcodec)
pkg_search_module(AVFORMAT REQUIRED libavformat)

if( OSX )
    find_library( COCOA_LIBRARIES NAMES Cocoa)
endif( OSX )

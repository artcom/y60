# This file resolves the dependencies of Y60

include(AcPlatform)

include(FindPkgConfig)

# art+com standard library
list(APPEND REQ_PACKAGES ASL)

# netsrc dependencies
list(APPEND REQ_PACKAGES Paintlib)
list(APPEND REQ_PACKAGES ACSDLTTF)
list(APPEND REQ_PACKAGES OSCPACK )

# javascript implementation
if (NOT Y60_WITH_TRACEMONKEY)
    list(APPEND REQ_PACKAGES Spidermonkey)
else (NOT Y60_WITH_TRACEMONKEY)
    list(APPEND REQ_PACKAGES MOZJS)
endif (NOT Y60_WITH_TRACEMONKEY)

# external dependencies
list(APPEND REQ_PACKAGES SDL GLEW NvidiaCg OpenEXR GLIB2)
list(APPEND OPT_PACKAGES Maya C4D)

# gtk and consorts
list(APPEND REQ_PKGCONFIG GTKMM gtkmm-2.4)
list(APPEND REQ_PKGCONFIG GLADEMM libglademm-2.4)
list(APPEND REQ_PKGCONFIG GTKGLEXT gtkglext-1.0)
list(APPEND OPT_PKGCONFIG GSTREAMER gstreamer-0.10)

# media codecs
list(APPEND REQ_PACKAGES FFMPEG )


if(WIN32)

    # pthread emulation library
    list(APPEND REQ_PACKAGES PthreadVC)

    # cairo backend
    list(APPEND REQ_PKGCONFIG CAIRO cairo-win32)
    
elseif(LINUX)

    # cairo backend
    list(APPEND REQ_PKGCONFIG CAIRO cairo-xlib)
    
elseif(OSX)

    # cairo backend
    list(APPEND REQ_PKGCONFIG CAIRO cairo-quartz)

    # cocoa ui framework
    find_library( COCOA_LIBRARIES NAMES Cocoa)
    mark_as_advanced( COCOA_LIBRARIES )
    
endif(WIN32)


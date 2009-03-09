# This file resolves the dependencies of Y60

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
list(APPEND OPT_PKGCONFIG GSTREAMER gstreamer-0.10)

list(APPEND OPT_PACKAGES GTKMM)
list(APPEND OPT_PACKAGES GTKGLEXT)

# media codecs
list(APPEND REQ_PACKAGES FFMPEG )


if(WIN32)

    # pthread emulation library
    list(APPEND REQ_PACKAGES PthreadVC)

    # we need some flavour of ATL for the WMV and WMA components
    list(APPEND REQ_PACKAGES ATL)

    # cairo backend
    list(APPEND REQ_PKGCONFIG CAIRO cairo-win32)

    list(APPEND REQ_PACKAGES Wmsdk)
    
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


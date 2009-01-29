if(WIN32)
    find_path( SWSCALE_INCLUDE_DIR swscale.h
            PATH_SUFFIXES libswscale ffmpeg)
    find_library( SWSCALE_LIBRARIES NAMES swscale libswscale)
    list( APPEND SWSCALE_LIBRARIES "${SWSCALE_LIBRARY}" )
else(WIN32)
    pkg_search_module(SWSCALE REQUIRED libswscale)
endif(WIN32)

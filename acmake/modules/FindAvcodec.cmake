if(WIN32)
    find_path( AVCODEC_INCLUDE_DIR avcodec.h
            PATH_SUFFIXES libavcodec ffmpeg)
    find_library( AVCODEC_LIBRARIES avcodec libavcodec)
else(WIN32)
    pkg_search_module(AVCODEC REQUIRED libavcodec)
endif(WIN32)

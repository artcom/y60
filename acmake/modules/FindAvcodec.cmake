if(WIN32)
    find_path( AVCODEC_INCLUDE_DIR avcodec.h
            PATH_SUFFIXES libavcodec ffmpeg)
    find_library( AVCODEC_LIBRARIES avcodec libavcodec)
    find_library( AVUTIL_LIBRARY avutil libavutil)
    list( APPEND AVFORMAT_LIBRARIES ${AVUTIL_LIBRARY} )
else(WIN32)
    pkg_search_module(AVCODEC REQUIRED libavcodec)
endif(WIN32)

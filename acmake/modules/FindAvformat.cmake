if(WIN32)
    find_path( AVFORMAT_INCLUDE_DIR avformat.h
            PATH_SUFFIXES libavformat ffmpeg)
    find_library( AVFORMAT_LIBRARIES avformat libavformat)
else(WIN32)
    pkg_search_module(AVFORMAT REQUIRED libavformat)
endif(WIN32)

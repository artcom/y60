if(WIN32)
    set(FFMPEG_SUBLIBS avcodec avformat avutil swscale)
    foreach(SUBLIB ${FFMPEG_SUBLIBS})
        string(TOUPPER ${SUBLIB} SUBLIB_UPPER) 
        find_path( ${SUBLIB_UPPER}_INCLUDE_DIRS ${SUBLIB}.h PATH_SUFFIXES lib${SUBLIB} ffmpeg)
        find_library( ${SUBLIB_UPPER}_LIBRARIES NAMES ${SUBLIB} lib${SUBLIB})
        mark_as_advanced( ${SUBLIB_UPPER}_LIBRARIES
                ${SUBLIB_UPPER}_INCLUDE_DIRS)
    endforeach(SUBLIB ${FFMPEG_SUBLIBS})
else(WIN32)
    pkg_search_module(AVCODEC REQUIRED libavcodec)
    pkg_search_module(AVFORMAT REQUIRED libavformat)
    pkg_search_module(SWSCALE libswscale)
    # contains av_free etc and is needed on Mac OS X. I think this is because of
    # incorrect linkage in the ffmpeg build. avformat etc should link avutil instead
    # of leaving the symbols undefined until the libs are linked (I think they use 
    # -undefined dynamic_lookup or something).
    # [DS]
    pkg_search_module(AVUTIL REQUIRED libavutil ) 
    pkg_search_module(AVFILTER libavfilter) # optional because it does not exist on linux
    set(FFMPEG_FOUND YES)
endif(WIN32)

set(FFMPEG_LIBRARIES ${AVCODEC_LIBRARIES} ${AVFORMAT_LIBRARIES} ${AVFILTER_LIBRARIES} ${SWSCALE_LIBRARIES} ${AVUTIL_LIBRARIES})
set(FFMPEG_LIBRARY_DIRS ${AVCODEC_LIBRARY_DIRS} ${AVFORMAT_LIBRARY_DIRS} ${AVFILTER_LIBRARY_DIRS} ${SWSCALE_LIBRARY_DIRS} ${AVUTIL_LIBRARY_DIRS})
set(FFMPEG_INCLUDE_DIRS ${AVCODEC_INCLUDE_DIRS} ${AVFORMAT_INCLUDE_DIRS} ${AVFILTER_INCLUDE_DIRS} ${SWSCALE_INCLUDE_DIRS} ${AVUTIL_INCLUDE_DIRS})


if( WIN32 )

    find_path(FFMPEC_INCLUDE_DIR ffmpec/avcodec.h )
              
    message("ffmpec include ${FFMPEC_INCLUDE_DIR}")
              
    set(FFMPEG_ALL_LIBRARIES avformat avcodec )
    
    foreach( SUBLIB ${FFMPEG_ALL_LIBRARIES} )
       mark_as_advanced( FFMPEG_SUBLIB_${SUBLIB} )
       find_library( FFMPEG_SUBLIB_${SUBLIB} NAMES ${SUBLIB} ) 
       if ( FFMPEG_SUBLIB_${SUBLIB}-NOTFOUND )
           set( FFMPEG-NOTFOUND TRUE )
           break()
       else ( FFMPEG_SUBLIB_${SUBLIB}-NOTFOUND )
           list( APPEND FFMPEG_LIBRARIES ${FFMPEG_SUBLIB_${SUBLIB}} )
       endif ( FFMPEG_SUBLIB_${SUBLIB}-NOTFOUND )
       
       message("ffmpeg ${SUBLIB} ${FFMPEG_SUBLIB_${SUBLIB}}")
       
    endforeach( SUBLIB ${FFMPEG_ALL_LIBRARIES} )
    
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(FFMPEG DEFAULT_MSG
            FFMPEG_LIBRARIES FFMPEG_INCLUDE_DIR)
    set(FFMPEG_DEFINITIONS -DFFMPEG_DLL )
    mark_as_advanced( FFMPEG_DEFINITIONS FFMPEG_INCLUDE_DIR FFMPEG_INCLUDE_DIRS )
    set(FFMPEG_INCLUDE_DIRS ${FFMPEG_INCLUDE_DIR})
    
else( WIN32 )
    pkg_search_module(AVCODEC REQUIRED libavcodec)
    pkg_search_module(AVFORMAT REQUIRED libavformat)
endif( WIN32 )




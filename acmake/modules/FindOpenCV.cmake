
FIND_PATH(OPENCV_INCLUDE_DIR cv.h cxcore.h
    PATH_SUFFIXES opencv
    PATHS
    "C:/OpenCV2.1/include"
    /usr/local/include
)

set(OPENCV_INCLUDE_DIRS ${OPENCV_INCLUDE_DIR})
mark_as_advanced(OPENCV_DEFINITIONS OPENCV_INCLUDE_DIR OPENCV_INCLUDE_DIRS)

if(WIN32) 
    set(OPENCV_SUBLIBS cv210.lib cvaux210.lib cxcore210.lib cxts210.lib highgui210.lib ml210.lib)
else(WIN32)
    set(OPENCV_SUBLIBS cv cvaux cxcore highgui ml)
endif(WIN32)

mark_as_advanced(OPENCV_SUBLIBS)

set(OPENCV_LIBRARIES)
foreach(SUBLIB ${OPENCV_SUBLIBS})
    mark_as_advanced(OPENCV_SUBLIB_${SUBLIB})
    find_library(OPENCV_SUBLIB_${SUBLIB} NAMES ${SUBLIB}  PATH_SUFFIXES lib PATHS "C:/OpenCV2.1" /usr/local/lib )
    list(APPEND OPENCV_LIBRARIES ${OPENCV_SUBLIB_${SUBLIB}})        
endforeach(SUBLIB ${OPENCV_ALL_SUBLIBS})
    

if(WIN32)
    set(OPENCV_SUBLIBS_D cv210d.lib cvaux210d.lib cxcore210d.lib cxts210d.lib highgui210d.lib ml210d.lib)
else(WIN32)
    set(OPENCV_SUBLIBS_D cv cvaux cxcore highgui ml)
endif(WIN32)
mark_as_advanced(OPENCV_SUBLIBS_D)

set(OPENCV_LIBRARIES_D)
foreach(SUBLIB ${OPENCV_SUBLIBS})
    mark_as_advanced(OPENCV_SUBLIB_${SUBLIB}_D)
    find_library(OPENCV_SUBLIB_${SUBLIB}_D NAMES ${SUBLIB}  PATH_SUFFIXES lib PATHS "C:/OpenCV2.1" /usr/local/lib )
    list(APPEND OPENCV_LIBRARIES_D ${OPENCV_SUBLIB_${SUBLIB}})        
endforeach(SUBLIB ${OPENCV_ALL_SUBLIBS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    OPENCV DEFAULT_MSG
    OPENCV_LIBRARIES OPENCV_LIBRARIES_D OPENCV_INCLUDE_DIR
)


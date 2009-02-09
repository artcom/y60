
set(MAYA_SDK $ENV{MAYA_SDK})
set(MAYA_VERSION $ENV{MAYA_VERSION})

if(MAYA_SDK AND MAYA_VERSION)
    set(MAYA_SUBLIBS Foundation OpenMaya OpenMayaAnim OpenMayaUI)
    
    find_path(MAYA_INCLUDE_DIR maya/MPoint.h 
        PATHS ENV MAYA_SDK
        PATH_SUFFIXES include
    )

    set(MAYA_INCLUDE_DIRS ${MAYA_INCLUDE_DIR})

    file(TO_CMAKE_PATH "${MAYA_SDK}/lib" MAYA_LIBRARY_DIR)

    set(MAYA_LIBRARIES)
    foreach(SUBLIB ${MAYA_SUBLIBS})
        string(TOUPPER ${SUBLIB} SUBLIB_UPPER)
        find_library(${SUBLIB_UPPER}_LOCATION NAMES ${SUBLIB}
            PATHS ENV MAYA_SDK
            PATH_SUFFIXES lib
        )
        list(APPEND MAYA_LIBRARIES ${SUBLIB})
    endforeach(SUBLIB ${MAYA_SUBLIBS})

    set(MAYA_DEFINITIONS -D_BOOL)

    set(MAYA_PLUGIN_DEFINITIONS)

    if(WIN32)
        list(APPEND MAYA_PLUGIN_DEFINITIONS -DNT_PLUGIN)
    endif(WIN32)

    if(APPLE)
        list(APPEND MAYA_PLUGIN_DEFINITIONS -DMAC_PLUGIN)
    endif(APPLE)

endif(MAYA_SDK AND MAYA_VERSION)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MAYA DEFAULT_MSG MAYA_INCLUDE_DIRS MAYA_LIBRARIES)

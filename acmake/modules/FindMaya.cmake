
set(MAYA_SDK $ENV{MAYA_SDK})
set(MAYA_VERSION $ENV{MAYA_VERSION})

if(MAYA_SDK AND MAYA_VERSION)

    # find includes  
    find_path(MAYA_INCLUDE_DIR maya/MPoint.h 
        PATHS ENV MAYA_SDK
        PATH_SUFFIXES include
    )
    
    set(MAYA_INCLUDE_DIRS ${MAYA_INCLUDE_DIR})
    mark_as_advanced(MAYA_INCLUDE_DIRS MAYA_INCLUDE_DIR)

    # find libraries
    file(TO_CMAKE_PATH "${MAYA_SDK}/lib" MAYA_LIBRARY_DIR)
    
    set(MAYA_SUBLIBS Foundation OpenMaya OpenMayaAnim OpenMayaUI)
    
    set(MAYA_LIBRARIES)
    foreach(SUBLIB ${MAYA_SUBLIBS})
        string(TOUPPER ${SUBLIB} SUBLIB_UPPER)
        
        mark_as_advanced(MAYA_SUBLIB_${SUBLIB_UPPER})
        
        find_library(MAYA_SUBLIB_${SUBLIB_UPPER} NAMES ${SUBLIB}
            PATHS ENV MAYA_SDK
            PATH_SUFFIXES lib
        )
    
        if(MAYA_SUBLIB_${SUBLIB_UPPER}-NOTFOUND)
            set(MAYA-NOTFOUND TRUE)
            break()
        else(MAYA_SUBLIB_${SUBLIB_UPPER}-NOTFOUND)
            list(APPEND MAYA_LIBRARIES ${MAYA_SUBLIB_${SUBLIB_UPPER}})
        endif(MAYA_SUBLIB_${SUBLIB_UPPER}-NOTFOUND)
        
        list(APPEND MAYA_LIBRARIES ${SUBLIB})
    endforeach(SUBLIB ${MAYA_SUBLIBS})
    mark_as_advanced(MAYA_LIBRARIES)

    # find executable
    file(TO_CMAKE_PATH "${MAYA_SDK}/bin" MAYA_EXECUTABLE_DIR)
    
    find_program(
        MAYA_EXECUTABLE
        NAMES maya maya2008 maya2009
        PATHS ${MAYA_EXECUTABLE_DIR} 
    )
    
    # determine definitions
    
    set(MAYA_DEFINITIONS -D_BOOL)
    
    set(MAYA_PLUGIN_DEFINITIONS ${MAYA_DEFINITIONS})
    
    if(WIN32)
        list(APPEND MAYA_PLUGIN_DEFINITIONS -DNT_PLUGIN)
    endif(WIN32)
    
    if(APPLE)
        list(APPEND MAYA_PLUGIN_DEFINITIONS -DMAC_PLUGIN)
    endif(APPLE)
    
endif(MAYA_SDK AND MAYA_VERSION)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MAYA DEFAULT_MSG MAYA_INCLUDE_DIRS MAYA_LIBRARIES MAYA_EXECUTABLE)

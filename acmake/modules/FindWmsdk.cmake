if(WIN32)

    find_package(WindowsSDK)

    find_library( 
        WMSDK_LIBRARY 
        NAMES WMVCORE wmvcore 
        PATHS "${WINDOWSSDK_DIR}/lib"
    )
    
    set(WMSDK_LIBRARIES ${WMSDK_LIBRARY})
    
    find_path(WMSDK_INCLUDE_DIR wmsdk.h PATHS "${WINDOWSSDK_DIR}/include")
    
    set(WMSDK_INCLUDE_DIRS ${WMSDK_INCLUDE_DIR})
    
    find_package_handle_standard_args(WMSDK DEFAULT_MSG WMSDK_LIBRARIES WMSDK_INCLUDE_DIRS)
    mark_as_advanced(WMSDK_LIBRARIES WMSDK_INCLUDE_DIRS)

endif(WIN32)

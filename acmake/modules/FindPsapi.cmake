if( WIN32 )

    find_package(WindowsSDK)
    
    find_library( 
        PSAPI_LIBRARY 
        NAMES Psapi 
        PATHS "${WINDOWSSDK_DIR}/lib"
    )
        
    set(PSAPI_LIBRARIES ${PSAPI_LIBRARY})
    
    find_path(PSAPI_INCLUDE_DIR Psapi.h PATHS "${WINDOWSSDK_DIR}/include")
    
    set(PSAPI_INCLUDE_DIRS ${PSAPI_INCLUDE_DIR})

    find_package_handle_standard_args(PSAPI DEFAULT_MSG PSAPI_LIBRARIES PSAPI_INCLUDE_DIRS)

    mark_as_advanced(PSAPI_LIBRARIES PSAPI_LIBRARY PSAPI_INCLUDE_DIRS PSAPI_INCLUDE_DIR)

endif(WIN32)

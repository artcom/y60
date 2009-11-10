# DirectShow GUIDs from the Windows SDK

if(WIN32)

    find_package(WindowsSDK)

    find_library( 
        STRMIIDS_LIBRARY 
        NAMES STRMIIDS strmiids 
        PATHS "${WINDOWSSDK_DIR}/lib"
    )
    
    set(STRMIIDS_LIBRARIES ${STRMIIDS_LIBRARY})
    
    find_path(STRMIIDS_INCLUDE_DIR initguid.h PATHS "${WINDOWSSDK_DIR}/include")
    
    set(STRMIIDS_INCLUDE_DIRS ${STRMIIDS_INCLUDE_DIR})
    
    find_package_handle_standard_args(STRMIIDS DEFAULT_MSG STRMIIDS_LIBRARIES STRMIIDS_INCLUDE_DIRS)

    mark_as_advanced(STRMIIDS_LIBRARIES STRMIIDS_LIBRARY STRMIIDS_INCLUDE_DIRS STRMIIDS_INCLUDE_DIR)

endif(WIN32)

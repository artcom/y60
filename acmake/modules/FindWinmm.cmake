if(WIN32)

    find_package(WindowsSDK)
    
    find_library( 
        WINMM_LIBRARY 
        NAMES Winmm WinMM 
        PATHS "${WINDOWSSDK_DIR}/lib"
    )
        
    set(WINMM_LIBRARIES ${WINMM_LIBRARY})
    
    find_package_handle_standard_args(WINMM DEFAULT_MSG WINMM_LIBRARIES)
    mark_as_advanced(WINMM_LIBRARIES)

endif(WIN32)

if( WIN32 )

    include(${CMAKE_BINARY_DIR}/VSVars.cmake)
    
    find_library( 
        WINMM_LIBRARY 
        NAMES Winmm WinMM 
        PATHS "${WINDOWSSDKDIR}Lib" )
        
    set( WINMM_LIBRARIES ${WINMM_LIBRARY} )
    
    find_package_handle_standard_args(WINMM DEFAULT_MSG WINMM_LIBRARIES )
    mark_as_advanced( WINMM_LIBRARIES  )

endif( WIN32 )

if( WIN32 )

    include(${CMAKE_BINARY_DIR}/VSVars.cmake)
    
    find_library( 
        PSAPI_LIBRARY 
        NAMES Psapi 
        PATHS "${WINDOWSSDKDIR}Lib" )
        
    set( PSAPI_LIBRARIES ${PSAPI_LIBRARY} )
    
    find_path( PSAPI_INCLUDE_DIR Psapi.h PATHS "${WINDOWSSDKDIR}Include" )
    
    set( PSAPI_INCLUDE_DIRS ${PSAPI_INCLUDE_DIR} )

    find_package_handle_standard_args(PSAPI DEFAULT_MSG PSAPI_LIBRARIES PSAPI_INCLUDE_DIRS )
    mark_as_advanced( PSAPI_LIBRARIES PSAPI_INCLUDE_DIRS )

endif( WIN32 )

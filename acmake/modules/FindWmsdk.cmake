if( WIN32 )

    include(${CMAKE_BINARY_DIR}/VSVars.cmake)

    find_library( 
        WMSDK_LIBRARY 
        NAMES WMVCORE wmvcore 
        PATHS "${CMAKE_SOURCE_DIR}/../extern/win/wmsdk/lib" "${WINDOWSSDKDIR}Lib"
    )
    
    set( WMSDK_LIBRARIES ${WMSDK_LIBRARY} )
    
    find_path( WMSDK_INCLUDE_DIR wmsdk.h PATHS "${CMAKE_SOURCE_DIR}/../extern/win/wmsdk/include"  "${WINDOWSSDKDIR}Include" )
    
    set( WMSDK_INCLUDE_DIRS ${WMSDK_INCLUDE_DIR} )
    
    find_package_handle_standard_args( WMSDK DEFAULT_MSG WMSDK_LIBRARIES WMSDK_INCLUDE_DIRS )
    mark_as_advanced( WMSDK_LIBRARIES WMSDK_INCLUDE_DIRS )

endif( WIN32 )

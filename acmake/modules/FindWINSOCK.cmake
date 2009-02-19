if( WIN32 )

    include(${CMAKE_BINARY_DIR}/VSVars.cmake)
   
    set(WINSOCK_ALL_LIBRARIES WS2_32 IPHlpApi )
    foreach( SUBLIB ${WINSOCK_ALL_LIBRARIES} )
        find_library( WINSOCK_SUBLIB_${SUBLIB} NAMES ${SUBLIB} PATHS "${WINDOWSSDKDIR}Lib" ) 
        if ( WINSOCK_SUBLIB_${SUBLIB}-NOTFOUND )
           set( WINSOCK-NOTFOUND TRUE )
           break()
        else ( WINSOCK_SUBLIB_${SUBLIB}-NOTFOUND )
           list( APPEND WINSOCK_LIBRARIES ${WINSOCK_SUBLIB_${SUBLIB}} )
        endif ( WINSOCK_SUBLIB_${SUBLIB}-NOTFOUND )
        mark_as_advanced( WINSOCK_SUBLIB_${SUBLIB} )
    endforeach( SUBLIB ${WINSOCK_ALL_LIBRARIES} )
 
    find_path( WINSOCK_INCLUDE_DIR IPHlpApi.h PATHS "${WINDOWSSDKDIR}Include" )
    
    set( WINSOCK_INCLUDE_DIRS ${WINSOCK_INCLUDE_DIR} )

    find_package_handle_standard_args(WINSOCK DEFAULT_MSG WINSOCK_LIBRARIES WINSOCK_INCLUDE_DIRS )
    mark_as_advanced( WINSOCK_LIBRARIES WINSOCK_INCLUDE_DIRS )

endif( WIN32 )

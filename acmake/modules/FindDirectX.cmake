if( WIN32 )

    find_path(DIRECTX_INCLUDE_DIR dxsdkver.h)
    mark_as_advanced(DIRECTX_INCLUDE_DIR)

    set(DIRECTX_SUBLIBS dsound dinput8 dxguid )
    mark_as_advanced(DIRECTX_SUBLIBS)

    set(DIRECTX_LIBRARIES)
    foreach(SUBLIB ${DIRECTX_SUBLIBS})
        mark_as_advanced(DIRECTX_SUBLIB_${SUBLIB})
        find_library(DIRECTX_SUBLIB_${SUBLIB} NAMES ${SUBLIB})
        if(DIRECTX_SUBLIB_${SUBLIB}-NOTFOUND)
            set(DIRECTX-NOTFOUND TRUE)
            break()
        else (DIRECTX_SUBLIB_${SUBLIB}-NOTFOUND)
            list(APPEND DIRECTX_LIBRARIES ${DIRECTX_SUBLIB_${SUBLIB}})
        endif(DIRECTX_SUBLIB_${SUBLIB}-NOTFOUND)
    endforeach(SUBLIB ${DIRECTX_ALL_LIBRARIES})
    
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(
        DIRECTX DEFAULT_MSG
        DIRECTX_LIBRARIES DIRECTX_INCLUDE_DIR
    )

endif( WIN32 )

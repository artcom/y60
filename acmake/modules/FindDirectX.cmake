if( WIN32 )

    set(DIRECTX_SDK_DIR $ENV{DXSDK_DIR})
    if(DIRECTX_SDK_DIR)
        list(APPEND CMAKE_PREFIX_PATH ${DIRECTX_SDK_DIR})
    endif(DIRECTX_SDK_DIR)
    mark_as_advanced(DIRECTX_SDK_DIR)

    find_path(DIRECTX_INCLUDE_DIR dxsdkver.h)
    mark_as_advanced(DIRECTX_INCLUDE_DIRS)

    set(DIRECTX_SUBLIBS dsound dinput8 dxguid )
    mark_as_advanced(DIRECTX_SUBLIBS)

    set(DIRECTX_LIBRARIES)
    foreach(SUBLIB ${DIRECTX_SUBLIBS})
        mark_as_advanced(DIRECTX_SUBLIB_${SUBLIB})
        find_library(DIRECTX_SUBLIB_${SUBLIB} NAMES ${SUBLIB} PATHS ${DIRECTX_SDK_DIR}/lib/x86)
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

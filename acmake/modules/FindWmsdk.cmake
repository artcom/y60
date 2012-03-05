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

    mark_as_advanced(WMSDK_LIBRARIES WMSDK_LIBRARY WMSDK_INCLUDE_DIRS WMSDK_INCLUDE_DIR)

    #check if we have Win7
    file (READ ${WMSDK_INCLUDE_DIR}/Msi.h MSI_H_CONTENT)
    string (REGEX MATCH "WIN7" IS_WIN_7 ${MSI_H_CONTENT})
    IF (IS_WIN_7)
        set_global (WINDOWS_7_SDK_FOUND TRUE)
    ELSE (IS_WIN_7)
        set_global (WINDOWS_7_SDK_FOUND FALSE)
    ENDIF (IS_WIN_7)

endif(WIN32)

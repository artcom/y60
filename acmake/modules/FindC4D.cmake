
set(C4D_SDK $ENV{C4D_SDK})
set(C4D_VERSION $ENV{C4D_VERSION})

if(C4D_SDK AND C4D_VERSION)

    find_path(C4D_INCLUDE_DIR c4d.h
        PATHS ENV C4D_SDK
        PATH_SUFFIXES _api
    )
    
    find_path(C4D_GV_INCLUDE_DIR ge_mtools.h
        PATHS ENV C4D_SDK
        PATH_SUFFIXES _api/c4d_gv
    )
    
    find_path(C4D_CUSTOMGUI_INCLUDE_DIR customgui_description.h
        PATHS ENV C4D_SDK
        PATH_SUFFIXES _api/c4d_customgui
    )
    
    find_path(C4D_LIBS_INCLUDE_DIR lib_description.h
        PATHS ENV C4D_SDK
        PATH_SUFFIXES _api/c4d_libs
    )

    set(C4D_INCLUDE_DIRS 
            ${C4D_INCLUDE_DIR} 
            ${C4D_GV_INCLUDE_DIR} 
            ${C4D_CUSTOMGUI_INCLUDE_DIR}
            ${C4D_LIBS_INCLUDE_DIR})

    mark_as_advanced(
        C4D_INCLUDE_DIRS
        C4D_INCLUDE_DIR
        C4D_GV_INCLUDE_DIR
        C4D_CUSTOMGUI_INCLUDE_DIR
    )

    find_library(C4D_LIBRARIES NAMES c4d_api.lib _api_Win32_Release.lib c4d_api_v8OPT)
    
    find_library(C4D_LIBRARIES_D NAMES c4d_apid.lib _api_Win32_Debug.lib c4d_api_v8DBG)

    mark_as_advanced(C4D_LIBRARIES C4D_LIBRARIES_D)

endif(C4D_SDK AND C4D_VERSION)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(C4D DEFAULT_MSG C4D_INCLUDE_DIRS C4D_LIBRARIES C4D_LIBRARIES_D)

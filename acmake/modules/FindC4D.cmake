
set(C4D_SDK $ENV{C4D_SDK})

if(C4D_SDK)

    find_path(C4D_INCLUDE_DIR c4d.h 
        PATHS ENV C4D_SDK
        PATH_SUFFIXES c4d
    )

    set(C4D_INCLUDE_DIRS ${C4D_INCLUDE_DIR})

    find_library(C4D_LIBRARIES NAMES c4d_api_v8OPT
            PATHS ENV C4D_SDK
    )
    
    find_library(C4D_LIBRARIES_D NAMES c4d_api_v8DBG
            PATHS ENV C4D_SDK
    )
    
    
    if(WIN32)
        set(C4D_DEFINITIONS -D__PC)
    endif(WIN32)
    if(OSX)
        set(C4D_DEFINITIONS -D__MAC)
    endif(OSX)

endif(C4D_SDK)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(C4D DEFAULT_MSG C4D_INCLUDE_DIR C4D_LIBRARIES)

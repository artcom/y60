if(WIN32)

    FIND_PATH(ALLIED_GIGABIT_INCLUDE_DIR PvApi.H
        PATH_SUFFIXES inc-pc
        PATHS
        "C:/Programme/Allied Vision Technologies/GigESDK/inc-pc")

    set(ALLIED_GIGABIT_INCLUDE_DIRS ${ALLIED_GIGABIT_INCLUDE_DIR})
    mark_as_advanced(ALLIED_GIGABIT_DEFINITIONS ALLIED_GIGABIT_INCLUDE_DIR ALLIED_GIGABIT_INCLUDE_DIRS)
    
    find_library(ALLIED_GIGABIT_LIBRARIES NAMES PvAPI.lib
        PATH_SUFFIXES lib-pc
        PATHS
        "C:/Programme/Allied Vision Technologies/GigESDK")

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(
        ALLIED_GIGABIT DEFAULT_MSG
        ALLIED_GIGABIT_LIBRARIES ALLIED_GIGABIT_INCLUDE_DIR
    )

endif(WIN32)

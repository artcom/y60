# Add win32 dependency folder
if(WIN32)
    set(PRO60_DEPS $ENV{PRO60_DEPS})
    if(PRO60_DEPS)
        list(INSERT CMAKE_PREFIX_PATH 0 ${PRO60_DEPS})
        add_subdirectory( ${PRO60_DEPS} ${CMAKE_CURRENT_BINARY_DIR}/pro60_deps )
    else(PRO60_DEPS)
        message(SEND_ERROR "PRO60_DEPS environment variable not set.")
    endif(PRO60_DEPS)
endif(WIN32)


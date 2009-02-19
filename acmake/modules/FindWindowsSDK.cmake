
if(WIN32)
    get_global(WINDOWSSDK_DIR WINDOWSSDK_DIR)

    if(WINDOWSSDK_DIR)
        set(WINDOWSSDK_FOUND TRUE)
    else(WINDOWSSDK_DIR)
        set(WINDOWSSDK_BOUNCEFILE "${CMAKE_CURRENT_BINARY_DIR}/WindowsSDK.cmake")

        if(NOT EXISTS ${WINDOWSSDK_BOUNCEFILE})
            set(VSCOMNTOOLS $ENV{VS90COMNTOOLS})
            if(NOT VSCOMNTOOLS)
                set(VSCOMNTOOLS $ENV{VS80COMNTOOLS})
            endif(NOT VSCOMNTOOLS)
            if(NOT VSCOMNTOOLS)
                set(VSCOMNTOOLS $ENV{VS70COMNTOOLS})
            endif(NOT VSCOMNTOOLS)

            if(NOT VSCOMNTOOLS)
                message(FATAL_ERROR "Could not determine VSCOMNTOOLS to use")
            endif(NOT VSCOMNTOOLS)

            execute_process(
                COMMAND ${ACMAKE_TOOLS_DIR}/FindWindowsSDK.bat  "${VSCOMNTOOLS}" "${WINDOWSSDK_BOUNCEFILE}"
                RESULT_VARIABLE RESULT
            )

            if(NOT RESULT EQUAL 0)
                message(FATAL_ERROR "Windows SDK location extraction failed")
            endif(NOT RESULT EQUAL 0)
        endif(NOT EXISTS ${WINDOWSSDK_BOUNCEFILE})

        include("${WINDOWSSDK_BOUNCEFILE}")

        file(TO_CMAKE_PATH "${WINDOWSSDK_DIR}" WINDOWSSDK_DIR)

        include(FindPackageHandleStandardArgs)
        find_package_handle_standard_args(
            WINDOWSSDK DEFAULT_MSG
            WINDOWSSDK_DIR
        )

        set_global(WINDOWSSDK_DIR ${WINDOWWSDK_DIR})
    endif(WINDOWSSDK_DIR)

endif(WIN32)

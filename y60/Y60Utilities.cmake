# Some utilities for building Y60-related things

find_package(AcMake)

function(y60_begin_application NAME)
    parse_arguments(
        APP
        "DISPLAY_NAME;DESCRIPTION;DEPENDS"
        ""
        ${ARGN}
    )

    # Check for nesting
    if(Y60_CURRENT_APPLICATION)
        message(FATAL_ERROR "Y60 application ${NAME} nested in ${Y60_CURRENT_APPLICATION}")
    endif(Y60_CURRENT_APPLICATION)

    set(Y60_CURRENT_APPLICATION ${NAME} PARENT_SCOPE)

    set_global(${NAME}_NAME ${NAME})

    set_global(${NAME}_IS_APPLICATION YES)

    set_global(${NAME}_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
    set_global(${NAME}_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

    set_global(${NAME}_BUILD_PATH   "${CMAKE_CURRENT_BINARY_DIR};${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_BINARY_DIR}/lib")
    set_global(${NAME}_INSTALL_PATH ".;../y60/components") # XXX: multi-conf

    # XXX: passthrough for late installer registration
    set_global(${NAME}_DISPLAY_NAME  ${APP_DISPLAY_NAME})
    set_global(${NAME}_DESCRIPTION   ${APP_DESCRIPTION})
    set_global(${NAME}_DEPENDS       ${APP_DEPENDS})
endfunction(y60_begin_application)

function(y60_add_asset FILE)
    set(APPLICATION ${Y60_CURRENT_APPLICATION})

    install(
        FILES "${FILE}"
        DESTINATION "lib/${APPLICATION}"
        COMPONENT ${APPLICATION}
    )
endfunction(y60_add_asset)

function(y60_add_assets DIRECTORY)
    parse_arguments(
        THIS_ASSETS
        "PATTERNS"
        "ADD_TO_PATH"
        ${ARGN}
    )

    set(APPLICATION ${Y60_CURRENT_APPLICATION})

    if(NOT THIS_ASSETS_PATTERNS)
        set(THIS_ASSETS_PATTERNS PATTERN "*")
    endif(NOT THIS_ASSETS_PATTERNS)

    get_filename_component(DIRECTORY_NAME "${DIRECTORY}" NAME)

    install(
        DIRECTORY "${DIRECTORY}"
        DESTINATION "lib/${APPLICATION}"
        COMPONENT ${APPLICATION}
        FILES_MATCHING
           ${THIS_ASSETS_PATTERNS}
           PATTERN ".svn" EXCLUDE
    )

    if(THIS_ASSETS_ADD_TO_PATH)
        append_global(${APPLICATION}_BUILD_PATH   "${DIRECTORY}")
        append_global(${APPLICATION}_INSTALL_PATH "${DIRECTORY_NAME}")
    endif(THIS_ASSETS_ADD_TO_PATH)
endfunction(y60_add_assets)

macro(y60_add_component COMPONENT_NAME)
    string(TOLOWER ${COMPONENT_NAME} COMPONENT_NAME_LOWER)
    # add aslbase to dependencies
    #list(APPEND THIS_PLUGIN_DEPENDS aslbase)

    ac_add_plugin(
        ${COMPONENT_NAME} y60/components
        RUNTIME_DEBIAN_PACKAGE     y60-plugin-${COMPONENT_NAME_LOWER}
        DEVELOPMENT_DEBIAN_PACKAGE y60-plugin-${COMPONENT_NAME_LOWER}-dev
        ${ARGN}
    )
endmacro(y60_add_component)

macro(y60_add_launcher NAME)
    parse_arguments(
        THIS_LAUNCHER
        "COMMAND_NAME;DESCRIPTION;CATEGORIES;ENGINE;MAIN_SCRIPT;BUILD_WORKING_DIR;INSTALL_WORKING_DIR;MIME_TYPES"
        ""
        ${ARGN}
    )

    set(APPLICATION ${Y60_CURRENT_APPLICATION})

    # if we are in an independent app, this will have been defined
    # if it wasn't, then get it from project globals
    if(NOT Y60_CMAKE_DIR)
        get_global(Y60_CMAKE_DIR Y60_CMAKE_DIR)
    endif(NOT Y60_CMAKE_DIR)

    get_global(${APPLICATION}_BUILD_PATH   THIS_APPLICATION_BUILD_PATH)
    get_global(${APPLICATION}_INSTALL_PATH THIS_APPLICATION_INSTALL_PATH)
    get_global(${APPLICATION}_BINARY_DIR   THIS_APPLICATION_BINARY_DIR)
    get_global(${APPLICATION}_SOURCE_DIR   THIS_APPLICATION_SOURCE_DIR)

    set(THIS_LAUNCHER_NAME "${NAME}")

    if(NOT THIS_LAUNCHER_ENGINE)
        set(THIS_LAUNCHER_ENGINE y60)
    endif(NOT THIS_LAUNCHER_ENGINE)

    if(NOT THIS_LAUNCHER_COMMAND_NAME)
        string(TOLOWER "${THIS_LAUNCHER_NAME}" THIS_LAUNCHER_COMMAND_NAME)
    endif(NOT THIS_LAUNCHER_COMMAND_NAME)

    # choose working dir for running from build tree
    if(THIS_LAUNCHER_BUILD_WORKING_DIR STREQUAL SOURCE)
        # BINARY lets us run in the source directory of the build
        set(THIS_LAUNCHER_BUILD_WORKING_DIR ${THIS_APPLICATION_SOURCE_DIR})
    elseif(THIS_LAUNCHER_BUILD_WORKING_DIR STREQUAL BINARY)
        # BINARY lets us run in the binary directory of the build
        set(THIS_LAUNCHER_BUILD_WORKING_DIR ${THIS_APPLICATION_BINARY_DIR})
    elseif(NOT THIS_LAUNCHER_BUILD_WORKING_DIR)
        # the prefered default is to run from ${PWD}
        set(THIS_LAUNCHER_BUILD_WORKING_DIR "")
    endif(THIS_LAUNCHER_BUILD_WORKING_DIR STREQUAL SOURCE)

    # choose working dir for running from install
    if(NOT THIS_LAUNCHER_INSTALL_WORKING_DIR)
        # the prefered default is to run from ${PWD}
        set(THIS_LAUNCHER_INSTALL_WORKING_DIR "")
    endif(NOT THIS_LAUNCHER_INSTALL_WORKING_DIR)

    get_property(THIS_LAUNCHER_BUILD_ENGINE TARGET ${THIS_LAUNCHER_ENGINE} PROPERTY LOCATION_RELEASE)
    set(THIS_LAUNCHER_INSTALL_ENGINE ${THIS_LAUNCHER_ENGINE})

    if(UNIX)
         # generate launcher shell script for build tree runs
         configure_file(
             ${Y60_CMAKE_DIR}/Y60BuildLauncher.sh.in
             ${CMAKE_CURRENT_BINARY_DIR}/${THIS_LAUNCHER_COMMAND_NAME}
             @ONLY
         )
         # generate launcher shell script for installed tree runs
         configure_file(
             ${Y60_CMAKE_DIR}/Y60InstallLauncher.sh.in
             ${CMAKE_CURRENT_BINARY_DIR}/${ACMAKE_BINARY_SUBDIR}/${THIS_LAUNCHER_COMMAND_NAME}
             @ONLY
         )
         install(
             FILES ${CMAKE_CURRENT_BINARY_DIR}/${ACMAKE_BINARY_SUBDIR}/${THIS_LAUNCHER_COMMAND_NAME}
             COMPONENT ${APPLICATION}
             DESTINATION bin/
             PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE
                         OWNER_READ    GROUP_READ    WORLD_READ
                         OWNER_WRITE
         )
    endif(UNIX)

    if(LINUX)
         # generate xdg desktop entry
         set(THIS_LAUNCHER_DESKTOP_FILE ${CMAKE_CURRENT_BINARY_DIR}/${THIS_LAUNCHER_COMMAND_NAME}.desktop)

         file(WRITE  ${THIS_LAUNCHER_DESKTOP_FILE} "[Desktop Entry]\n")
         file(APPEND ${THIS_LAUNCHER_DESKTOP_FILE} "Type=Application\n")
         file(APPEND ${THIS_LAUNCHER_DESKTOP_FILE} "Name=${THIS_LAUNCHER_NAME}\n")
         file(APPEND ${THIS_LAUNCHER_DESKTOP_FILE} "TryExec=${THIS_LAUNCHER_COMMAND_NAME}\n")
         file(APPEND ${THIS_LAUNCHER_DESKTOP_FILE} "Exec=${THIS_LAUNCHER_COMMAND_NAME} %U\n")
         file(APPEND ${THIS_LAUNCHER_DESKTOP_FILE} "Categories=${THIS_LAUNCHER_CATEGORIES}\n")
         if(THIS_LAUNCHER_MIME_TYPES)
             file(APPEND ${THIS_LAUNCHER_DESKTOP_FILE} "MimeType=${THIS_LAUNCHER_MIME_TYPES}\n")
         endif(THIS_LAUNCHER_MIME_TYPES)

         install(
             FILES ${THIS_LAUNCHER_DESKTOP_FILE}
             COMPONENT ${APPLICATION}
             DESTINATION share/applications
             PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE
                         OWNER_READ    GROUP_READ    WORLD_READ
                         OWNER_WRITE
         )
    endif(LINUX)

    if(WIN32)
         # generate launcher batch script for installed tree runs
         configure_file(
             ${Y60_CMAKE_DIR}/Y60InstallLauncher.bat.in
             ${CMAKE_CURRENT_BINARY_DIR}/${ACMAKE_BINARY_SUBDIR}/${NAME}.bat
             @ONLY
         )
         install(
             FILES ${CMAKE_CURRENT_BINARY_DIR}/${ACMAKE_BINARY_SUBDIR}/${NAME}.bat
             COMPONENT ${APPLICATION}
             DESTINATION bin/
             PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE
                         OWNER_READ    GROUP_READ    WORLD_READ
                         OWNER_WRITE
         )

         ac_add_installer_shortcut(${NAME} "" "" "" "bin\\\\\\\\${NAME}.bat" "" "")
    endif(WIN32)
endmacro(y60_add_launcher)

macro(y60_add_watchdog_launcher NAME)
    parse_arguments(
        THIS_LAUNCHER
        "BINARY_NAME;DESCRIPTION;CONFIG_FILE;INSTALL_DIR;MIME_TYPES"
        ""
        ${ARGN}
    )

    set(APPLICATION ${Y60_CURRENT_APPLICATION})

    # if we are in an independent app, this will have been defined
    # if it wasn't, then get it from project globals
    if(NOT Y60_CMAKE_DIR)
        get_global(Y60_CMAKE_DIR Y60_CMAKE_DIR)
    endif(NOT Y60_CMAKE_DIR)

    get_global(${APPLICATION}_BUILD_PATH   THIS_APPLICATION_BUILD_PATH)
    get_global(${APPLICATION}_INSTALL_PATH THIS_APPLICATION_INSTALL_PATH)
    get_global(${APPLICATION}_BINARY_DIR   THIS_APPLICATION_BINARY_DIR)
    get_global(${APPLICATION}_SOURCE_DIR   THIS_APPLICATION_SOURCE_DIR)

    set(THIS_LAUNCHER_NAME "${NAME}")

    if(NOT THIS_LAUNCHER_BINARY_NAME)
        set(THIS_LAUNCHER_BINARY_NAME watchdog)
    endif(NOT THIS_LAUNCHER_BINARY_NAME)

    if(NOT THIS_LAUNCHER_CONFIG_FILE)
        set(THIS_LAUNCHER_CONFIG_FILE watchdog.xml)
    endif(NOT THIS_LAUNCHER_CONFIG_FILE)
    y60_add_asset(${THIS_LAUNCHER_CONFIG_FILE})

    if(NOT THIS_LAUNCHER_INSTALL_DIR)
        set(THIS_LAUNCHER_INSTALL_DIR /opt/ART+COM)
    endif(NOT THIS_LAUNCHER_INSTALL_DIR)

    if(UNIX)
        set(THIS_LAUNCHER_LAUNCH_FILE ${CMAKE_CURRENT_BINARY_DIR}/${ACMAKE_BINARY_SUBDIR}/${THIS_LAUNCHER_NAME})
        file(WRITE  ${THIS_LAUNCHER_LAUNCH_FILE} "#!/bin/sh\n")
        file(APPEND ${THIS_LAUNCHER_LAUNCH_FILE} "cd ../lib/${APPLICATION}\n")
        file(APPEND ${THIS_LAUNCHER_LAUNCH_FILE} "${THIS_LAUNCHER_BINARY_NAME} --configfile ${THIS_LAUNCHER_CONFIG_FILE}\n")
        install(
             FILES ${THIS_LAUNCHER_LAUNCH_FILE}
             COMPONENT ${APPLICATION}
             DESTINATION bin/
             PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE
                         OWNER_READ    GROUP_READ    WORLD_READ
                         OWNER_WRITE
        )
    endif(UNIX)

    if(LINUX)
         # generate xdg desktop entry
         set(THIS_LAUNCHER_DESKTOP_FILE ${CMAKE_CURRENT_BINARY_DIR}/${THIS_LAUNCHER_NAME}.desktop)

         file(WRITE  ${THIS_LAUNCHER_DESKTOP_FILE} "[Desktop Entry]\n")
         file(APPEND ${THIS_LAUNCHER_DESKTOP_FILE} "Type=Application\n")
         file(APPEND ${THIS_LAUNCHER_DESKTOP_FILE} "Name=${THIS_LAUNCHER_NAME}\n")
         file(APPEND ${THIS_LAUNCHER_DESKTOP_FILE} "Path=${THIS_LAUNCHER_INSTALL_DIR}/${APPLICATION}\n")
         file(APPEND ${THIS_LAUNCHER_DESKTOP_FILE} "TryExec=${THIS_LAUNCHER_BINARY_NAME}\n")
         file(APPEND ${THIS_LAUNCHER_DESKTOP_FILE} "Exec=${THIS_LAUNCHER_BINARY_NAME} --configfile ${THIS_LAUNCHER_CONFIG_FILE} %U\n")
         if(THIS_LAUNCHER_MIME_TYPES)
             file(APPEND ${THIS_LAUNCHER_DESKTOP_FILE} "MimeType=${THIS_LAUNCHER_MIME_TYPES}\n")
         endif(THIS_LAUNCHER_MIME_TYPES)

         install(
             FILES ${THIS_LAUNCHER_DESKTOP_FILE}
             COMPONENT ${APPLICATION}
             DESTINATION share/applications
             PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE
                         OWNER_READ    GROUP_READ    WORLD_READ
                         OWNER_WRITE
         )
    endif(LINUX)

    if(WIN32)
        # TODO: last argument COMMAND_DIR is a relative path to the watchdog,
        # ...this is a hack, and should be replaced by something proper (GM)
        ac_add_installer_shortcut("${NAME}" "" "" "lib\\\\\\\\${APPLICATION}"  "${THIS_LAUNCHER_BINARY_NAME}" "--configfile ${THIS_LAUNCHER_CONFIG_FILE}" "\\\\\\\\..\\\\\\\\PRO60\\\\\\\\bin\\\\\\\\")
    endif(WIN32)
endmacro(y60_add_watchdog_launcher)

function(y60_end_application NAME)
    if(NOT Y60_CURRENT_APPLICATION)
        message(FATAL_ERROR "Trying to end Y60 application ${NAME} before it started")
    endif(NOT Y60_CURRENT_APPLICATION)
    get_global(${NAME}_BUILD_PATH BUILD_PATH)
    get_global(${NAME}_INSTALL_PATH INSTALL_PATH)

    get_global(${NAME}_DISPLAY_NAME  APP_DISPLAY_NAME)
    get_global(${NAME}_DESCRIPTION   APP_DESCRIPTION)
    get_global(${NAME}_DEPENDS       APP_DEPENDS)

    set(Y60_CURRENT_APPLICATION)
endfunction(y60_end_application)

# select build type for running tests
if(CMAKE_BUILD_TYPE)
    string(TOUPPER "${CMAKE_BUILD_TYPE}" Y60_TEST_BUILD_TYPE)
else(CMAKE_BUILD_TYPE)
    set(Y60_TEST_BUILD_TYPE RELEASE CACHE STRING "Build type to use for running tests")
endif(CMAKE_BUILD_TYPE)

# macro for adding javascript execution tests
macro(y60_add_jstest NAME PREFIX)
    # find y60 executable
    get_target_property(Y60_EXECUTABLE y60 LOCATION_${Y60_TEST_BUILD_TYPE})
    
    # path with test-specifics
    set(_PATH ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

    # register the test
    add_test(${PREFIX}_${NAME}
        ${Y60_EXECUTABLE} test${NAME}.tst.js ${ARGN} shaderlibrary.xml -I "${_PATH}"
    )
endmacro(y60_add_jstest)

# macro for adding render tests
macro(y60_add_rendertest NAME)
    parse_arguments(
        TEST
        "LOAD;IMAGE_PREFIX;IMAGE_SUFFIX;BASELINE_DIR;OUTPUT_DIR;TOLERANCE;THRESHOLD"
        ""
        ${ARGN}
    )

    if(NOT TEST_OUTPUT_DIR)
        set(TEST_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
    endif(NOT TEST_OUTPUT_DIR)

    if(NOT TEST_TOLERANCE)
        set(TEST_TOLERANCE "0.12")
    endif(NOT TEST_TOLERANCE)

    if(NOT TEST_THRESHOLD)
        set(TEST_THRESHOLD "30")
    endif(NOT TEST_THRESHOLD)

    set(TEST_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    set(TEST_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

    get_target_property(
        TEST_Y60_EXECUTABLE
        y60
        LOCATION_${Y60_TEST_BUILD_TYPE}
    )
    get_target_property(
        TEST_Y60_COMPARE_IMAGE_EXECUTABLE
        compareimage
        LOCATION_${Y60_TEST_BUILD_TYPE}
    )

    set(TEST_Y60_PATH ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

    if(NOT Y60_CMAKE_DIR)
        get_global(Y60_CMAKE_DIR Y60_CMAKE_DIR)
    endif(NOT Y60_CMAKE_DIR)

    configure_file(
        ${Y60_CMAKE_DIR}/Y60RenderTest.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/test_${NAME}.cmake
        @ONLY
    )

    add_test(
        render_${NAME}
        cmake -P ${CMAKE_CURRENT_BINARY_DIR}/test_${NAME}.cmake
    )
endmacro(y60_add_rendertest)

if(WIN32)
    set(MAYA_MAKE nmake /f)
else(WIN32)
    set(MAYA_MAKE make -f)
endif(WIN32)

macro(y60_maya_to_scene MAYA_FILE)
    parse_arguments(
        THIS
        "OUTPUT"
        "BINARY;INLINE_TEXTURES"
        ${ARGN}
    )

    if(MAYA_FOUND)

        set(FILE "${CMAKE_BINARY_DIR}/${ACMAKE_BINARY_SUBDIR}/MayaConversions.make")

        get_global(Y60_MAYA_LIST_STARTED FLAG)
        if(NOT FLAG)
            file(WRITE ${FILE} "# This generated makefile converts maya scenes to y60\n")
            file(APPEND ${FILE} "default: all\n")

            add_custom_target(
                maya-export ALL
                ${MAYA_MAKE} ${FILE}
            )

            set_global(Y60_MAYA_LIST_STARTED YES)
        endif(NOT FLAG)

        if(NOT THIS_OUTPUT)
            get_filename_component(THIS_OUTPUT ${MAYA_FILE} NAME_WE)
            if(THIS_BINARY)
                set(THIS_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${THIS_OUTPUT}.b60")
            else(THIS_BINARY)
                set(THIS_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${THIS_OUTPUT}.x60")
            endif(THIS_BINARY)
        endif(NOT THIS_OUTPUT)
        
        set(I_BINARY)
        if(THIS_BINARY)
            set(I_BINARY "binary=1")
        endif(THIS_BINARY)
        
        set(I_INLINE_TEXTURES)
        if(THIS_INLINE_TEXTURES)
            set(I_INLINE_TEXTURES "inlineTextures=1")
        endif(THIS_INLINE_TEXTURES)

        file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" MK_WORKDIR)
        file(TO_CMAKE_PATH "${THIS_OUTPUT}" MK_OUTPUT)
        file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${MAYA_FILE}" MK_INPUT)

        file(APPEND ${FILE}
            "${MK_OUTPUT}: ${MK_INPUT}\n")
        file(APPEND ${FILE}
            "\t\"${MAYA_EXECUTABLE}\" -batch -proj \"${MK_WORKDIR}\" -file \"${MK_INPUT}\" -command \"file -op \\\"${I_BINARY};${I_INLINE_TEXTURES};progressBar=0\\\" -f -ea -type \\\"Y60\\\" \\\"${MK_OUTPUT}\\\"\"\n")
        file(APPEND ${FILE}
            "all:: ${MK_OUTPUT}\n")
        
    else(MAYA_FOUND)
        message(WARNING "Can't convert ${MAYA_FILE} to scene format without Maya")
    endif(MAYA_FOUND)
endmacro(y60_maya_to_scene)

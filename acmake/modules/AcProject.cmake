# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#
# Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
#
# This file is part of the ART+COM CMake Library (acmake).
#
# It is distributed under the Boost Software License, Version 1.0. 
# (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)             
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#
#
# Project handling.
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

# XXX: document members
set(_AC_PROJECT_VARIABLES
    NAME FOUND IS_PROJECT
    IS_IMPORTED IS_INTEGRATED
    BUILD_TYPE
    BINARY_DIR SOURCE_DIR INSTALL_PREFIX
    RUNTIME_DIR LIBRARY_DIR INCLUDE_DIR
    LIBRARIES EXECUTABLES
    PLUGIN_DIRS PLUGINS
    EXTERNS
)

# XXX: document members
set(_AC_EXTERN_VARIABLES
    IS_PACKAGE IS_PKGCONFIG
    PKGCONFIG
)

macro(ac_add_project PROJECT_NAME)
    parse_arguments(THIS_PROJECT
        "REQUIRED_PACKAGES;OPTIONAL_PACKAGES;REQUIRED_PKGCONFIG;OPTIONAL_PKGCONFIG"
        ";"
        ${ARGN})

    # Enter project context
    set(ACMAKE_CURRENT_PROJECT ${PROJECT_NAME})

    # Remember the binary and source dir of this project
    set_global(${PROJECT_NAME}_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    set_global(${PROJECT_NAME}_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    
    # Also remember the build type
    set_global(${PROJECT_NAME}_BUILD_TYPE "${CMAKE_BUILD_TYPE}")

    # We are of course FOUND
    set_global(${PROJECT_NAME}_FOUND         YES)

    # Mark our namespace as a project
    set_global(${PROJECT_NAME}_IS_PROJECT    YES)

    # As we are defined here, we are not imported but integrated
    set_global(${PROJECT_NAME}_IS_IMPORTED   NO )
    set_global(${PROJECT_NAME}_IS_INTEGRATED YES)

    # Alias the install prefix into the project namespace
    set_global(${PROJECT_NAME}_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

    # Our actual install destinations
    set_global(${PROJECT_NAME}_RUNTIME_DIR ${CMAKE_INSTALL_PREFIX}/bin)
    set_global(${PROJECT_NAME}_LIBRARY_DIR ${CMAKE_INSTALL_PREFIX}/lib)
    set_global(${PROJECT_NAME}_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)

    # Plugin install directories will be collected here
    set_global(${PROJECT_NAME}_PLUGIN_DIRS "")

    # Variables to collect targets into
    set_global(${PROJECT_NAME}_LIBRARIES   "")
    set_global(${PROJECT_NAME}_EXECUTABLES "")
    set_global(${PROJECT_NAME}_PLUGINS     "")

    # External dependencies
    set_global(${PROJECT_NAME}_EXTERNS     "")

    # Add required and optional cmake-packaged externals
    foreach(PACKAGE ${THIS_PROJECT_REQUIRED_PACKAGES})
        ac_project_add_extern_package(${PACKAGE} REQUIRED)
    endforeach(PACKAGE ${THIS_PROJECT_REQUIRED_PACKAGES})
    foreach(PACKAGE ${THIS_PROJECT_OPTIONAL_PACKAGES})
        ac_project_add_extern_package(${PACKAGE})
    endforeach(PACKAGE ${THIS_PROJECT_OPTIONAL_PACKAGES})

    # Add required and optional pkgconfig packages
    if(THIS_PROJECT_REQUIRED_PKGCONFIG)
        set(LST ${THIS_PROJECT_REQUIRED_PKGCONFIG})
        while(LST)
            list(GET LST 0 PKG_PREFIX)
            list(GET LST 1 PKG_PACKAGE)
            list(REMOVE_AT LST 0)
            list(REMOVE_AT LST 0)

            ac_project_add_extern_pkgconfig(${PKG_PREFIX} ${PKG_PACKAGE} REQUIRED)
        endwhile(LST)
    endif(THIS_PROJECT_REQUIRED_PKGCONFIG)
    if(THIS_PROJECT_OPTIONAL_PKGCONFIG)
        set(LST ${THIS_PROJECT_OPTIONAL_PKGCONFIG})
        while(LST)
            list(GET LST 0 PKG_PREFIX)
            list(GET LST 1 PKG_PACKAGE)
            list(REMOVE_AT LST 0)
            list(REMOVE_AT LST 0)

            ac_project_add_extern_pkgconfig(${PKG_PREFIX} ${PKG_PACKAGE})
        endwhile(LST)
    endif(THIS_PROJECT_OPTIONAL_PKGCONFIG)

endmacro(ac_add_project PROJECT_NAME)

macro(ac_project_add_target TARGET_TYPE_PLURAL TARGET_NAME)
    parse_arguments(THIS_TARGET
        "EXTERNS;DEPENDS"
        ""
        ${ARGN}
    )
    if(ACMAKE_CURRENT_PROJECT)
        append_global(${ACMAKE_CURRENT_PROJECT}_${TARGET_TYPE_PLURAL} ${TARGET_NAME})

        set_global(${ACMAKE_CURRENT_PROJECT}_${TARGET_NAME}_LIBRARIES    ${TARGET_NAME})
        set_global(${ACMAKE_CURRENT_PROJECT}_${TARGET_NAME}_DEFINITIONS  "")
        set_global(${ACMAKE_CURRENT_PROJECT}_${TARGET_NAME}_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)
        set_global(${ACMAKE_CURRENT_PROJECT}_${TARGET_NAME}_LIBRARY_DIRS ${CMAKE_INSTALL_PREFIX}/lib)

        foreach(EXTERN ${THIS_TARGET_EXTERNS})
#            append_global(${ACMAKE_CURRENT_PROJECT}_${TARGET_NAME}_LIBRARIES ${${EXTERN}_LIBRARIES})
#            append_global(${ACMAKE_CURRENT_PROJECT}_${TARGET_NAME}_INCLUDE_DIRS ${${EXTERN}_INCLUDE_DIRS})
#            append_global(${ACMAKE_CURRENT_PROJECT}_${TARGET_NAME}_LIBRARY_DIRS ${${EXTERN}_LIBRARY_DIRS})
            append_global(${ACMAKE_CURRENT_PROJECT}_${TARGET_NAME}_EXTERNS ${EXTERN})
        endforeach(EXTERN ${THIS_TARGET_EXTERNS})
        foreach(DEPEND ${THIS_TARGET_DEPENDS})
            append_global(${ACMAKE_CURRENT_PROJECT}_${TARGET_NAME}_DEPENDS ${DEPEND})
        endforeach(DEPEND ${THIS_TARGET_DEPENDS})
    endif(ACMAKE_CURRENT_PROJECT)
endmacro(ac_project_add_target TARGET_TYPE_PLURAL TARGET_NAME)

macro(ac_project_add_extern_package PACKAGE_NAME)
    parse_arguments(THIS_PACKAGE
        ""
        "REQUIRED"
        ${ARGN}
    )
    if(ACMAKE_CURRENT_PROJECT)
        set(PACKAGE_REQUIRED)
        if(THIS_PACKAGE_REQUIRED)
            set(PACKAGE_REQUIRED REQUIRED)
        endif(THIS_PACKAGE_REQUIRED)
        find_package(${PACKAGE_NAME} ${PACKAGE_REQUIRED})
        append_global(${ACMAKE_CURRENT_PROJECT}_EXTERNS ${PACKAGE_NAME})
        set_global(${ACMAKE_CURRENT_PROJECT}_EXTERN_${PACKAGE_NAME}_IS_PACKAGE YES)
    endif(ACMAKE_CURRENT_PROJECT)
endmacro(ac_project_add_extern_package PACKAGE_NAME)

macro(ac_project_add_extern_pkgconfig PACKAGE_NAME PKGCONFIG_NAME)
    parse_arguments(THIS_PKGCONFIG
        ""
        "REQUIRED"
        ${ARGN}
    )
    if(ACMAKE_CURRENT_PROJECT)
        set(PACKAGE_REQUIRED)
        if(THIS_PKGCONFIG_REQUIRED)
            set(PACKAGE_REQUIRED REQUIRED)
        endif(THIS_PKGCONFIG_REQUIRED)
        pkg_search_module(${PACKAGE_NAME} ${PKGCONFIG_REQUIRED} ${PKGCONFIG_NAME})
        if(${PACKAGE_NAME}_FOUND)
            append_global(${ACMAKE_CURRENT_PROJECT}_EXTERNS ${PACKAGE_NAME})
            set_global(${ACMAKE_CURRENT_PROJECT}_EXTERN_${PACKAGE_NAME}_IS_PKGCONFIG YES)
            set_global(${ACMAKE_CURRENT_PROJECT}_EXTERN_${PACKAGE_NAME}_PKGCONFIG    ${PKGCONFIG_NAME})
        endif(${PACKAGE_NAME}_FOUND)
    endif(ACMAKE_CURRENT_PROJECT)
endmacro(ac_project_add_extern_pkgconfig PACKAGE_NAME)

macro(ac_export_project PROJECT_NAME)
    get_globals(
        ${PROJECT_NAME}
        THIS_PROJECT
        ${_AC_PROJECT_VARIABLES}
    )

    # XXX: remove this line
    set(THIS_PROJECT_NAME ${PROJECT_NAME})

    # Generate Find-file
    configure_file(
        ${ACMAKE_TEMPLATES_DIR}/AcFind.cmake.in
        ${THIS_PROJECT_BINARY_DIR}/Find${PROJECT_NAME}.cmake
        @ONLY
    )

    # Generate dependency finding script
    set(EXT "${THIS_PROJECT_BINARY_DIR}/Find${PROJECT_NAME}Dependencies.cmake")
    file(WRITE ${EXT} "# This generated script resolves external dependencies for ${PROJECT_NAME}\n")
    file(WRITE ${EXT} "include(FindPkgConfig)\n")
    foreach(PACKAGE ${THIS_PROJECT_EXTERNS})
        get_globals(ASL_EXTERN_${PACKAGE} THIS_PACKAGE ${_AC_EXTERN_VARIABLES})

        if(THIS_PACKAGE_IS_PACKAGE)
            file(APPEND ${EXT} "find_package(${PACKAGE} REQUIRED)\n")
        endif(THIS_PACKAGE_IS_PACKAGE)

        if(THIS_PACKAGE_IS_PKGCONFIG)
            file(APPEND ${EXT} "pkg_search_module(${PACKAGE} REQUIRED ${THIS_PACKAGE_PKGCONFIG})\n")
        endif(THIS_PACKAGE_IS_PKGCONFIG)

        clear_locals(THIS_PACKAGE ${AC_EXTERN_VARIABLES})
    endforeach(PACKAGE ${THIS_PROJECT_EXTERNS})

    # Generate target dependency script
    set(EXT "${THIS_PROJECT_BINARY_DIR}/Find${PROJECT_NAME}Targets.cmake")
    file(WRITE ${EXT} "# This generated script defines target dependencies for ${PROJECT_NAME}\n")
    foreach(LIBRARY ${THIS_PROJECT_LIBRARIES})
        get_globals(${PROJECT_NAME}_${LIBRARY} THIS_LIBRARY "LIBRARIES;DEFINITIONS;INCLUDE_DIRS;LIBRARY_DIRS;DEPENDS;EXTERNS")
        file(APPEND ${EXT} "# library target ${LIBRARY}\n")
        file(APPEND ${EXT} "set(${LIBRARY}_PROJECT ${PROJECT_NAME})\n")
        file(APPEND ${EXT} "set(${LIBRARY}_DEPENDS ${THIS_LIBRARY_DEPENDS})\n")
        file(APPEND ${EXT} "set(${LIBRARY}_EXTERNS ${THIS_LIBRARY_EXTERNS})\n")
    endforeach(LIBRARY ${THIS_PROJECT_LIBRARIES})

    # Install find fileset
    install(
        FILES
            ${THIS_PROJECT_BINARY_DIR}/Find${PROJECT_NAME}.cmake
            ${THIS_PROJECT_BINARY_DIR}/Find${PROJECT_NAME}Dependencies.cmake
            ${THIS_PROJECT_BINARY_DIR}/Find${PROJECT_NAME}Targets.cmake
	    DESTINATION share/cmake-2.6/Modules
    )

    # Remove our THIS_ variables
    clear_locals(THIS_PROJECT ${_AC_PROJECT_VARIABLES})

    # Leave the project context
    set(ACMAKE_CURRENT_PROJECT)
endmacro(ac_export_project PROJECT_NAME)

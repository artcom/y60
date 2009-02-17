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
# This subsystem has the purpose of collecting targets into
# a set for packaging. It resolves external dependencies and
# generates a set of find-files for other projects to include.
#
# TODO:
#  - rework dependency handling to be compatible
#    with pkg-config version expressions and more
#    advanced packages with arguments like FindBoost
#  - implement cpack support
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

option(ACMAKE_DEBUG_EXPORTS "Debug acmake project export mechanism" NO)
macro(debug_exports)
    if(ACMAKE_DEBUG_EXPORTS)
        message("EXPORTS: ${ARGN}")
    endif(ACMAKE_DEBUG_EXPORTS)
endmacro(debug_exports)

# Class declaration for projects.
#
# This will later be used with VariableUtils to
# copy the object between the various namespaces
# we have to put it in. (XXX: elaborate somewhere)
#
set(_AC_PROJECT_VARIABLES
    NAME           # project's name
    FOUND          # find_package-like found marker
    IS_PROJECT     # class tag (for "type checks")
    IS_IMPORTED    # true if project was loaded
    IS_INTEGRATED  # true if project is part of this build
    BUILD_TYPE     # cmake build type the project was built with (XXX: not with the VS-build)
    BINARY_DIR     # root of binary build tree
    SOURCE_DIR     # root of source tree
    INSTALL_PREFIX # install prefix the project was compiled for
    RUNTIME_DIR    # absolute install location of binaries (and dlls on windows)
    LIBRARY_DIR    # absolute install location of libraries
    INCLUDE_DIR    # absolute install location of headers
    DEFINITIONS    # definitions our clients should use when including us
    LIBRARIES      # list of all libraries in the project
    EXECUTABLES    # list of all executables in the project
    PLUGIN_DIRS    # XXX: not implemented. should be involved in y60 in-tree PATH generation
    PLUGINS        # list of all plugins in the project
    EXTERNS        # list of all externs used by project
    REQUIRED_PACKAGES # list of all required find packages
    OPTIONAL_PACKAGES # list of all optional find packages
    REQUIRED_PKGCONFIG # bilist of all required pkg-config packages
    OPTIONAL_PKGCONFIG # bilist of all optional pkg-config packages
    CUSTOM_SCRIPTS # custom scripts to be included in the find-package
)

# Static method: check if a project is integrated or not
macro(ac_is_integrated PROJECT_NAME OUT)
    get_global(${PROJECT_NAME}_IS_INTEGRATED _IS)
    set(OUT ${_IS})
    set(_IS)
endmacro(ac_is_integrated)

# Add a project to the current build.
#
# This takes a name and some keyword arguments declaring
# external dependencies. (XXX: This mechanism sucks)
#
macro(ac_add_project PROJECT_NAME)
    parse_arguments(THIS_PROJECT
        "REQUIRED_PACKAGES;OPTIONAL_PACKAGES;REQUIRED_PKGCONFIG;OPTIONAL_PKGCONFIG;CUSTOM_SCRIPTS;DEFINITIONS"
        ";"
        ${ARGN})

    # Check for nesting
    if(ACMAKE_CURRENT_PROJECT)
        message(FATAL_ERROR "Project ${PROJECT_NAME} nested in ${ACMAKE_CURRENT_PROJECT}")
    endif(ACMAKE_CURRENT_PROJECT)

    # Enter project context
    set(ACMAKE_CURRENT_PROJECT ${PROJECT_NAME})

    # Set the name of the object
    set_global(${PROJECT_NAME}_NAME ${PROJECT_NAME})

    # Remember the binary and source dir of this project
    set_global(${PROJECT_NAME}_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    set_global(${PROJECT_NAME}_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    
    # Also remember the build type (if we do not have one, like in VS, just leave it empty)
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

    # Definitions for our clients
    set_global(${PROJECT_NAME}_DEFINITIONS ${THIS_PROJECT_DEFINITIONS})

    # Plugin install directories will be collected here
    set_global(${PROJECT_NAME}_PLUGIN_DIRS "")

    # Variables to collect targets into
    set_global(${PROJECT_NAME}_LIBRARIES   "")
    set_global(${PROJECT_NAME}_EXECUTABLES "")
    set_global(${PROJECT_NAME}_PLUGINS     "")

    # Externs used by us
    set_global(${PROJECT_NAME}_EXTERNS     "")

    # External dependencies
    set_global(${PROJECT_NAME}_REQUIRED_PACKAGES "")
    set_global(${PROJECT_NAME}_OPTIONAL_PACKAGES "")
    set_global(${PROJECT_NAME}_REQUIRED_PKGCONFIG "")
    set_global(${PROJECT_NAME}_OPTIONAL_PKGCONFIG "")

    # Remember custom scripts
    set_global(${PROJECT_NAME}_CUSTOM_SCRIPTS ${THIS_PROJECT_CUSTOM_SCRIPTS})

    # Add required cmake-packaged externals
    foreach(PACKAGE ${THIS_PROJECT_REQUIRED_PACKAGES})
        ac_project_add_extern_package(${PACKAGE} REQUIRED)
    endforeach(PACKAGE ${THIS_PROJECT_REQUIRED_PACKAGES})

    # Add optional cmake-packaged externals
    foreach(PACKAGE ${THIS_PROJECT_OPTIONAL_PACKAGES})
        ac_project_add_extern_package(${PACKAGE})
    endforeach(PACKAGE ${THIS_PROJECT_OPTIONAL_PACKAGES})

    # Add required pkgconfig packages
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

    # Add optional pkgconfig packages
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

    # Add project-wide cpp definitions
    # XXX: This is a bad place for this
    add_definitions(${THIS_PROJECT_DEFINITIONS})

endmacro(ac_add_project PROJECT_NAME)

# Internal method for adding targets into the current project.
#
macro(ac_project_add_target TARGET_TYPE_PLURAL TARGET_NAME)
    parse_arguments(THIS_TARGET
        "EXTERNS;DEPENDS"
        ""
        ${ARGN}
    )

    # if we are not in a project, we ignore the call
    if(ACMAKE_CURRENT_PROJECT)
        debug_exports("Registering target ${TARGET_NAME} into ${TARGET_TYPE_PLURAL} of ${ACMAKE_CURRENT_PROJECT}")

        # mark the target as coming from this project
        set_global(${TARGET_NAME}_PROJECT ${ACMAKE_CURRENT_PROJECT})

        # add the target to the projects list for the appropriate type
        append_global(${ACMAKE_CURRENT_PROJECT}_${TARGET_TYPE_PLURAL} ${TARGET_NAME})

        # fill the target object
        set_global(${ACMAKE_CURRENT_PROJECT}_${TARGET_NAME}_LIBRARIES    ${TARGET_NAME})
        set_global(${ACMAKE_CURRENT_PROJECT}_${TARGET_NAME}_DEPENDS      "")
        set_global(${ACMAKE_CURRENT_PROJECT}_${TARGET_NAME}_EXTERNS      "")
        set_global(${ACMAKE_CURRENT_PROJECT}_${TARGET_NAME}_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)
        set_global(${ACMAKE_CURRENT_PROJECT}_${TARGET_NAME}_LIBRARY_DIRS ${CMAKE_INSTALL_PREFIX}/lib)

        # add externs to project object
        foreach(EXTERN ${THIS_TARGET_EXTERNS})
            append_global_unique(${ACMAKE_CURRENT_PROJECT}_EXTERNS ${EXTERN})
        endforeach(EXTERN ${THIS_TARGET_EXTERNS})

        # add externs to target object
        foreach(EXTERN ${THIS_TARGET_EXTERNS})
            debug_exports("Target depends on extern ${EXTERN}")
            append_global_unique(${ACMAKE_CURRENT_PROJECT}_${TARGET_NAME}_EXTERNS ${EXTERN})
        endforeach(EXTERN ${THIS_TARGET_EXTERNS})

        # add externs to target object
        foreach(DEPEND ${THIS_TARGET_DEPENDS})
            debug_exports("Target depends on depend ${DEPEND}")
            append_global_unique(${ACMAKE_CURRENT_PROJECT}_${TARGET_NAME}_DEPENDS ${DEPEND})
        endforeach(DEPEND ${THIS_TARGET_DEPENDS})

        # for each depend, add the externs it pulls in
        foreach(DEPEND ${THIS_TARGET_DEPENDS})
            # we need to know from which project the depend comes
            get_global(${DEPEND}_PROJECT _DEPEND_PROJECT )
            # then we get its externs list
            get_global(${_DEPEND_PROJECT}_${DEPEND}_EXTERNS _IMPLICIT_EXTERNS )
            # and add it if there is anything to add
            if(_IMPLICIT_EXTERNS)
                debug_exports("Found ${DEPEND} to require ${_IMPLICIT_EXTERNS}")
                append_global_unique(${ACMAKE_CURRENT_PROJECT}_${TARGET_NAME}_EXTERNS ${_IMPLICIT_EXTERNS})
            endif(_IMPLICIT_EXTERNS)
        endforeach(DEPEND ${THIS_TARGET_DEPENDS})
    endif(ACMAKE_CURRENT_PROJECT)
endmacro(ac_project_add_target TARGET_TYPE_PLURAL TARGET_NAME)

# Internal method for adding find_package-dependencies to the current project
#
macro(ac_project_add_extern_package PACKAGE_NAME)
    parse_arguments(THIS_PACKAGE
        ""
        "REQUIRED"
        ${ARGN}
    )

    # if we are not in a project, we ignore the call
    if(ACMAKE_CURRENT_PROJECT)
        # process REQUIRED argument
        set(PACKAGE_REQUIRED)
        if(THIS_PACKAGE_REQUIRED)
            set(PACKAGE_REQUIRED REQUIRED)
            set_global(${ACMAKE_CURRENT_PROJECT}_EXTERN_${PACKAGE_NAME}_IS_REQUIRED YES)
        endif(THIS_PACKAGE_REQUIRED)
        
        # load the package description (XXX: wart)
        find_package(${PACKAGE_NAME} ${PACKAGE_REQUIRED})
        
        # register the dependency (XXX: no found check, relevant for optionals)
        if(THIS_PACKAGE_REQUIRED)
            append_global(${ACMAKE_CURRENT_PROJECT}_REQUIRED_PACKAGES ${PACKAGE_NAME})
        else(THIS_PACKAGE_REQUIRED)
            append_global(${ACMAKE_CURRENT_PROJECT}_OPTIONAL_PACKAGES ${PACKAGE_NAME})
        endif(THIS_PACKAGE_REQUIRED)
        set_global(${ACMAKE_CURRENT_PROJECT}_EXTERN_${PACKAGE_NAME}_IS_PACKAGE YES)
    endif(ACMAKE_CURRENT_PROJECT)
endmacro(ac_project_add_extern_package PACKAGE_NAME)

# Internal method for adding pkg-config-dependencies to the current project
#
macro(ac_project_add_extern_pkgconfig PACKAGE_NAME PKGCONFIG_NAME)
    parse_arguments(THIS_PKGCONFIG
        ""
        "REQUIRED"
        ${ARGN}
    )

    # if we are not in a project, we ignore the call
    if(ACMAKE_CURRENT_PROJECT)
        # process REQUIRED argument
        set(PACKAGE_REQUIRED)
        if(THIS_PKGCONFIG_REQUIRED)
            set(PACKAGE_REQUIRED REQUIRED)
            set_global(${ACMAKE_CURRENT_PROJECT}_EXTERN_${PACKAGE_NAME}_IS_REQUIRED YES)
        endif(THIS_PKGCONFIG_REQUIRED)

        # load the pkg-config module (XXX: wart)
        pkg_search_module(${PACKAGE_NAME} ${PKGCONFIG_REQUIRED} ${PKGCONFIG_NAME})

        # register the dependency if we found it
        if(${PACKAGE_NAME}_FOUND)
            if(THIS_PKGCONFIG_REQUIRED)
                append_global(${ACMAKE_CURRENT_PROJECT}_REQUIRED_PKGCONFIG ${PACKAGE_NAME} ${PKGCONFIG_NAME})
            else(THIS_PKGCONFIG_REQUIRED)
                append_global(${ACMAKE_CURRENT_PROJECT}_OPTIONAL_PKGCONFIG ${PACKAGE_NAME} ${PKGCONFIG_NAME})
            endif(THIS_PKGCONFIG_REQUIRED)
            set_global(${ACMAKE_CURRENT_PROJECT}_EXTERN_${PACKAGE_NAME}_IS_PKGCONFIG YES)
            set_global(${ACMAKE_CURRENT_PROJECT}_EXTERN_${PACKAGE_NAME}_PKGCONFIG ${PKGCONFIG_NAME})
        endif(${PACKAGE_NAME}_FOUND)
    endif(ACMAKE_CURRENT_PROJECT)
endmacro(ac_project_add_extern_pkgconfig PACKAGE_NAME)

# End declaration of a project.
#
# This is where find-files are generated.
#
macro(ac_end_project PROJECT_NAME)
    # Check if we are ending the right project
    if(NOT "${PROJECT_NAME}" STREQUAL "${ACMAKE_CURRENT_PROJECT}")
        message(FATAL_ERROR "Ending a project that is not the current project.")
    endif(NOT "${PROJECT_NAME}" STREQUAL "${ACMAKE_CURRENT_PROJECT}")

    # Bring object into local namespace
    get_globals(
        ${PROJECT_NAME}
        THIS_PROJECT
        ${_AC_PROJECT_VARIABLES}
    )

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
    foreach(PACKAGE ${THIS_PROJECT_REQUIRED_PACKAGES})
        file(APPEND ${EXT} "find_package(${PACKAGE} REQUIRED)\n")
    endforeach(PACKAGE ${THIS_PROJECT_REQUIRED_PACKAGES})
    foreach(PACKAGE ${THIS_PROJECT_OPTIONAL_PACKAGES})
        file(APPEND ${EXT} "find_package(${PACKAGE})\n")
    endforeach(PACKAGE ${THIS_PROJECT_OPTIONAL_PACKAGES})

    set(_LST ${THIS_PROJECT_REQUIRED_PKGCONFIG})
    while(_LST)
        list(GET _LST 0 PKG_PREFIX)
        list(GET _LST 1 PKG_PACKAGE)
        list(REMOVE_AT _LST 0)
        list(REMOVE_AT _LST 0)
        
        file(APPEND ${EXT} "pkg_search_module(${PKG_PREFIX} REQUIRED ${PKG_PACKAGE})\n")
    endwhile(_LST)

    set(_LST ${THIS_PROJECT_OPTIONAL_PKGCONFIG})
    while(_LST)
        list(GET _LST 0 PKG_PREFIX)
        list(GET _LST 1 PKG_PACKAGE)
        list(REMOVE_AT _LST 0)
        list(REMOVE_AT _LST 0)
        
        file(APPEND ${EXT} "pkg_search_module(${PKG_PREFIX} ${PKG_PACKAGE})\n")
    endwhile(_LST)

    # Generate target declaration script
    # XXX: not implemented for plugins
    set(EXT "${THIS_PROJECT_BINARY_DIR}/Find${PROJECT_NAME}Targets.cmake")
    file(WRITE ${EXT} "# This generated script defines target dependencies for ${PROJECT_NAME}\n")
    foreach(LIBRARY ${THIS_PROJECT_LIBRARIES})
        get_globals(${PROJECT_NAME}_${LIBRARY} THIS_LIBRARY "LIBRARIES;INCLUDE_DIRS;LIBRARY_DIRS;DEPENDS;EXTERNS") # XXX: classify
        file(APPEND ${EXT} "# library target ${LIBRARY}\n")
        file(APPEND ${EXT} "set(${LIBRARY}_PROJECT ${PROJECT_NAME})\n")
        file(APPEND ${EXT} "set(${LIBRARY}_DEPENDS ${THIS_LIBRARY_DEPENDS})\n")
        file(APPEND ${EXT} "set(${LIBRARY}_EXTERNS ${THIS_LIBRARY_EXTERNS})\n")
    endforeach(LIBRARY ${THIS_PROJECT_LIBRARIES})

    # Preprocess list of custom scripts
    # XXX: dirty hack
    set(THIS_PROJECT_CUSTOM_SCRIPT_FILES)
    foreach(SCRIPT ${THIS_PROJECT_CUSTOM_SCRIPTS})
        list(APPEND THIS_PROJECT_CUSTOM_SCRIPT_FILES ${SCRIPT}.cmake)
    endforeach(SCRIPT ${THIS_PROJECT_CUSTOM_SCRIPTS})

    # Install find fileset
    install(
        FILES
            ${THIS_PROJECT_BINARY_DIR}/Find${PROJECT_NAME}.cmake
            ${THIS_PROJECT_BINARY_DIR}/Find${PROJECT_NAME}Dependencies.cmake
            ${THIS_PROJECT_BINARY_DIR}/Find${PROJECT_NAME}Targets.cmake
            ${THIS_PROJECT_CUSTOM_SCRIPT_FILES}
	    DESTINATION share/cmake-2.6/Modules
    )

    # Remove our local copy of the project
    clear_locals(THIS_PROJECT ${_AC_PROJECT_VARIABLES})

    # Leave the project context
    set(ACMAKE_CURRENT_PROJECT)
endmacro(ac_end_project PROJECT_NAME)

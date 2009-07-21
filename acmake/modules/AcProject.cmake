# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#
# Copyright (C) 1993-2009, ART+COM AG Berlin, Germany <www.artcom.de>
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
#  - rework package dependency handling to be compatible
#    with pkg-config version expressions and more
#    advanced packages with arguments like FindBoost
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

# define debug channels
ac_define_debug_channel(project "Debug project analysis")
ac_define_debug_channel(exports "Debug project exports")

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
    SOURCE_DIR     # location of source tree (DO NOT TEMPLATE)
    BINARY_DIR     # location of binary tree (DO NOT TEMPLATE)
    RUNTIME_DIR    # absolute install location of binaries (and dlls on windows)
    LIBRARY_DIR    # absolute install location of libraries
    INCLUDE_DIR    # absolute install location of headers
    BUILD_INCLUDE_DIRS # include dirs in build tree
    CMAKE_DIR      # absolute build/install location of cmake files
    CMAKE_SUBDIR   # subdir of cmake files in both install and build trees
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
    CUSTOM_SCRIPTS   # custom scripts to be loaded by cmake config
    CUSTOM_TEMPLATES # custom templates for use by cmake configs
    VENDOR           # string describing the vendor
    CONTACT          # contact information (email address)
    DESCRIPTION      # short human-readable description
    DESCRIPTION_FILE # build-time location of description file
    LICENSE_FILE     # build-time location of license file
)

# Static method: check if a project is integrated or not
macro(ac_is_integrated PROJECT_NAME OUT)
    get_global(${PROJECT_NAME}_IS_INTEGRATED _IS)
    set(OUT ${_IS})
    set(_IS)
endmacro(ac_is_integrated)

# list of arguments and flags to project declarations
# externalized to reduce clutter in the declaration macro
set(PROJECT_ARGUMENTS
    REQUIRED_PACKAGES
    OPTIONAL_PACKAGES
    REQUIRED_PKGCONFIG
    OPTIONAL_PKGCONFIG
    CUSTOM_SCRIPTS
    CUSTOM_TEMPLATES
    DEFINITIONS
    # NOTE: installer stuff follows
    VENDOR
    CONTACT
    DESCRIPTION
    DESCRIPTION_FILE
    LICENSE_FILE
)
set(PROJECT_FLAGS
)

# Add a project to the current build.
#
# This takes a name and some keyword arguments declaring
# external dependencies. (XXX: This mechanism sucks)
#
macro(ac_add_project PROJECT_NAME)
    parse_arguments(THIS_PROJECT
        "${PROJECT_ARGUMENTS}"
        "${PROJECT_FLAGS}"
        ${ARGN}
    )

    ac_debug_project("${PROJECT_NAME} begins")

    # Check for nesting
    if(ACMAKE_CURRENT_PROJECT)
        message(FATAL_ERROR "Project ${PROJECT_NAME} nested in ${ACMAKE_CURRENT_PROJECT}")
    endif(ACMAKE_CURRENT_PROJECT)

    # Enter project context
    set(ACMAKE_CURRENT_PROJECT ${PROJECT_NAME})

    # generate lowercase name for use in paths
    string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)

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

    # decide where cmake files should go
    if(WIN32)
        set(THIS_PROJECT_CMAKE_SUBDIR "cmake")
    elseif(OSX)
        set(THIS_PROJECT_CMAKE_SUBDIR ".")
    elseif(LINUX)
        set(THIS_PROJECT_CMAKE_SUBDIR "lib/${PROJECT_NAME_LOWER}/cmake")
    endif(WIN32)
    set_global(${PROJECT_NAME}_CMAKE_SUBDIR "${THIS_PROJECT_CMAKE_SUBDIR}")
    set(THIS_PROJECT_CMAKE_DIR "${CMAKE_BINARY_DIR}/${THIS_PROJECT_CMAKE_SUBDIR}")
    set_global(${PROJECT_NAME}_CMAKE_DIR    "${THIS_PROJECT_CMAKE_DIR}")

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

    # Remember custom files
    set_global(${PROJECT_NAME}_CUSTOM_SCRIPTS   ${THIS_PROJECT_CUSTOM_SCRIPTS})
    set_global(${PROJECT_NAME}_CUSTOM_TEMPLATES ${THIS_PROJECT_CUSTOM_TEMPLATES})

    # Remember various installer-related things
    set_global(${PROJECT_NAME}_VENDOR           ${THIS_PROJECT_VENDOR})
    set_global(${PROJECT_NAME}_CONTACT          ${THIS_PROJECT_CONTACT})
    set_global(${PROJECT_NAME}_DESCRIPTION      ${THIS_PROJECT_DESCRIPTION})
    set_global(${PROJECT_NAME}_DESCRIPTION_FILE ${THIS_PROJECT_DESCRIPTION_FILE})
    set_global(${PROJECT_NAME}_LICENSE_FILE     ${THIS_PROJECT_LICENSE_FILE})

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
    # XXX: It might be better to attach this directly to targets
    #      instead of propagating it with the directory hierarchy.
    add_definitions(${THIS_PROJECT_DEFINITIONS})

    # Integrate with the installation framework
    _ac_package_project(
        ${PROJECT_NAME}
        VENDOR           ${THIS_PROJECT_VENDOR}
        CONTACT          ${THIS_PROJECT_CONTACT}
        DESCRIPTION      ${THIS_PROJECT_DESCRIPTION}
        DESCRIPTION_FILE ${THIS_PROJECT_DESCRIPTION_FILE}
        LICENSE_FILE     ${THIS_PROJECT_LICENSE_FILE}
    )

    # Load custom scripts
    foreach(SCRIPT ${THIS_PROJECT_CUSTOM_SCRIPTS})
        include(${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT})
    endforeach(SCRIPT)

    # Make custom scripts available to build-tree clients
    foreach(FILE ${THIS_PROJECT_CUSTOM_SCRIPTS} ${THIS_PROJECT_CUSTOM_TEMPLATES})
        configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}"
            "${THIS_PROJECT_CMAKE_DIR}/${FILE}"
            COPYONLY
        )
    endforeach(FILE)
    
endmacro(ac_add_project PROJECT_NAME)

macro(ac_project_add_build_include_dir PATH)
    append_global(${ACMAKE_CURRENT_PROJECT}_BUILD_INCLUDE_DIRS ${PATH})
    include_directories(${PATH})
endmacro(ac_project_add_build_include_dir)

# Internal method for adding targets to the current project.
#
macro(ac_project_add_target TARGET_TYPE_PLURAL TARGET_NAME)
    parse_arguments(THIS_TARGET
        "EXTERNS;DEPENDS"
        ""
        ${ARGN}
    )

    # if we are not in a project, we ignore the call
    if(ACMAKE_CURRENT_PROJECT)
        ac_debug_project("registering target ${TARGET_NAME} with project ${ACMAKE_CURRENT_PROJECT} under ${TARGET_TYPE_PLURAL}")

        # mark the target as coming from this project
        set_global(${TARGET_NAME}_PROJECT ${ACMAKE_CURRENT_PROJECT})

        # add the target to the projects list for the appropriate type
        append_global(${ACMAKE_CURRENT_PROJECT}_${TARGET_TYPE_PLURAL} ${TARGET_NAME})

        # fill out the target object
        set_global(${TARGET_NAME}_LIBRARIES    ${TARGET_NAME})
        set_global(${TARGET_NAME}_DEPENDS      "")
        set_global(${TARGET_NAME}_EXTERNS      "")
        set_global(${TARGET_NAME}_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)
        set_global(${TARGET_NAME}_LIBRARY_DIRS ${CMAKE_INSTALL_PREFIX}/lib)

        # add externs to target object
        if(THIS_TARGET_EXTERNS)
            ac_debug_exports("target has externs ${THIS_TARGET_EXTERNS}")

            # add externs to project object
            append_global_unique(${ACMAKE_CURRENT_PROJECT}_EXTERNS ${THIS_TARGET_EXTERNS})

            # add externs to target object
            append_global_unique(${TARGET_NAME}_EXTERNS ${THIS_TARGET_EXTERNS})
        endif(THIS_TARGET_EXTERNS)

        # add depends to target object
        if(THIS_TARGET_DEPENDS)
            ac_debug_exports("target has depends: ${THIS_TARGET_DEPENDS}")

            foreach(DEPEND ${THIS_TARGET_DEPENDS})
                append_global_unique(${TARGET_NAME}_DEPENDS ${DEPEND})
            endforeach(DEPEND ${THIS_TARGET_DEPENDS})
        endif(THIS_TARGET_DEPENDS)

        # for each depend, add the externs it pulls in
        set(_ALL_IMPLICIT_EXTERNS)
        foreach(DEPEND ${THIS_TARGET_DEPENDS})
            # we need to know from which project the depend comes
            get_global(${DEPEND}_PROJECT _DEPEND_PROJECT)
            # then we get its externs list
            get_global(${_DEPEND_PROJECT}_${DEPEND}_EXTERNS _IMPLICIT_EXTERNS)
            # and add it if there is anything to add
            if(_IMPLICIT_EXTERNS)
                append_global_unique(${TARGET_NAME}_EXTERNS ${_IMPLICIT_EXTERNS})
                list(APPEND _ALL_IMPLICIT_EXTERNS ${_IMPLICIT_EXTERNS})
            endif(_IMPLICIT_EXTERNS)
        endforeach(DEPEND ${THIS_TARGET_DEPENDS})
        if(_ALL_IMPLICIT_EXTERNS)
            ac_debug_exports("target has transient externs: ${_ALL_IMPLICIT_EXTERNS}")
        endif(_ALL_IMPLICIT_EXTERNS)
    endif(ACMAKE_CURRENT_PROJECT)
endmacro(ac_project_add_target TARGET_TYPE_PLURAL TARGET_NAME)

# Internal method for adding find_package-dependencies to the current project
#
# XXX: this is a wart and should be replaced with a project-scoped macro
#      that allows passing more arguments to the search call
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
        
        # load the package description
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
# XXX: this is a wart and should be replaced with a project-scoped macro
#      that allows passing more arguments to the search call
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

        # load the module
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

# Helper for emitting a property into a file,
# stuffing it both into the global and scoped namespace.
# XXX: This is a kludge prompting for a cleanup of AcFind.cmake.in.
macro(ac_emit_property FILE NAME)
    file(APPEND ${FILE} "set(${NAME} \"${ARGN}\")\n")
    file(APPEND ${FILE} "set_property(GLOBAL PROPERTY ${NAME} \"${ARGN}\")\n")
endmacro(ac_emit_property)

# End declaration of a project.
#
# This is where find-files are generated.
#
macro(ac_end_project PROJECT_NAME)
    # Check if we are ending the right project
    if(NOT "${PROJECT_NAME}" STREQUAL "${ACMAKE_CURRENT_PROJECT}")
        message(FATAL_ERROR "Ending a project that is not the current project.")
    endif(NOT "${PROJECT_NAME}" STREQUAL "${ACMAKE_CURRENT_PROJECT}")

    # Generate lowercase name
    string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)

    # Bring object into local namespace
    get_globals(
        ${PROJECT_NAME}
        THIS_PROJECT
        ${_AC_PROJECT_VARIABLES}
    )

    # Generate Find-file
    ac_configure_file(
        ${ACMAKE_TEMPLATES_DIR}/AcFind.cmake.in
        ${THIS_PROJECT_CMAKE_DIR}/${PROJECT_NAME}Config.cmake
        "ac_end_project()"
    )

    # Generate dependency finding script
    set(EXT "${THIS_PROJECT_CMAKE_DIR}/${PROJECT_NAME}Dependencies.cmake")
    file(WRITE ${EXT} "# This generated script resolves external dependencies for ${PROJECT_NAME}\n")
    file(WRITE ${EXT} "include(FindPkgConfig)\n")

    # load lines for find packages
    foreach(PACKAGE ${THIS_PROJECT_REQUIRED_PACKAGES})
        file(APPEND ${EXT} "find_package(${PACKAGE} REQUIRED)\n")
    endforeach(PACKAGE ${THIS_PROJECT_REQUIRED_PACKAGES})
    foreach(PACKAGE ${THIS_PROJECT_OPTIONAL_PACKAGES})
        file(APPEND ${EXT} "find_package(${PACKAGE})\n")
    endforeach(PACKAGE ${THIS_PROJECT_OPTIONAL_PACKAGES})

    # load lines for pkgconfig packages
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

    # Generate target information script (XXX: plugin export not implemented)
    set(EXT "${THIS_PROJECT_CMAKE_DIR}/${PROJECT_NAME}Targets.cmake")
    file(WRITE ${EXT} "# This generated script defines target dependencies for ${PROJECT_NAME}\n")
    foreach(EXECUTABLE ${THIS_PROJECT_EXECUTABLES})
        get_globals(${EXECUTABLE} THIS_EXECUTABLE "LIBRARIES;INCLUDE_DIRS;LIBRARY_DIRS;DEPENDS;EXTERNS") # XXX: make class for this

        if(WIN32)
            set(LOCATION  "bin/${EXECUTABLE}.exe")
        else(WIN32)
            set(LOCATION "bin/${EXECUTABLE}")
        endif(WIN32)

        file(APPEND ${EXT} "# executable target ${EXECUTABLE}\n")
        ac_emit_property(${EXT} ${EXECUTABLE}_PROJECT ${PROJECT_NAME})
        ac_emit_property(${EXT} ${EXECUTABLE}_LOCATION "${LOCATION}")
    endforeach(EXECUTABLE ${THIS_PROJECT_EXECUTABLES})
    foreach(LIBRARY ${THIS_PROJECT_LIBRARIES})
        get_globals(${LIBRARY} THIS_LIBRARY "LIBRARIES;INCLUDE_DIRS;LIBRARY_DIRS;DEPENDS;EXTERNS") # XXX: make class for this

        if(WIN32)
            set(OBJECT_LOCATION  "bin/${LIBRARY}.dll")
            set(IMPLIB_LOCATION "lib/${LIBRARY}.lib")
        else(WIN32)
            set(OBJECT_LOCATION "lib/${CMAKE_SHARED_MODULE_PREFIX}${LIBRARY}${CMAKE_SHARED_MODULE_SUFFIX}")
            set(IMPLIB_LOCATION)
        endif(WIN32)

        file(APPEND ${EXT} "# library target ${LIBRARY}\n")
        ac_emit_property(${EXT} ${LIBRARY}_PROJECT ${PROJECT_NAME})
        ac_emit_property(${EXT} ${LIBRARY}_DEPENDS ${THIS_LIBRARY_DEPENDS})
        ac_emit_property(${EXT} ${LIBRARY}_EXTERNS ${THIS_LIBRARY_EXTERNS})
        ac_emit_property(${EXT} ${LIBRARY}_OBJECT_LOCATION "${OBJECT_LOCATION}")
        if(IMPLIB_LOCATION)
            ac_emit_property(${EXT} ${LIBRARY}_IMPLIB_LOCATION "${IMPLIB_LOCATION}")
        endif(IMPLIB_LOCATION)
    endforeach(LIBRARY ${THIS_PROJECT_LIBRARIES})

    # Install cmake files
    install(
        FILES
            "${THIS_PROJECT_CMAKE_DIR}/${PROJECT_NAME}Config.cmake"
            "${THIS_PROJECT_CMAKE_DIR}/${PROJECT_NAME}Dependencies.cmake"
            "${THIS_PROJECT_CMAKE_DIR}/${PROJECT_NAME}Targets.cmake"
            ${THIS_PROJECT_CUSTOM_SCRIPTS}
            ${THIS_PROJECT_CUSTOM_TEMPLATES}
	    DESTINATION ${THIS_PROJECT_CMAKE_SUBDIR}
        COMPONENT ${PROJECT_NAME}_development
    )

    # Remove our local copy of the project
    clear_locals(THIS_PROJECT ${_AC_PROJECT_VARIABLES})

    # Leave the project context
    set(ACMAKE_CURRENT_PROJECT)

    ac_debug_project("${PROJECT_NAME} has ended")
endmacro(ac_end_project PROJECT_NAME)

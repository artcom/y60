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
# Macro to declare an executable within the current acmake project
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

# XXX: HEADERS is allowed as an argument. This should be removed.

# Define an executable in the current ACMake project.
#
# ac_add_executable(
#    name
#    SOURCES sourcefile ...
#    [ DEPENDS integrated_library ... ]
#    [ EXTERNS imported_package ... ]
#    [ DONT_INSTALL ]
# )
#
# The NAME is used in project exports and for the target name.
#
# SOURCES is a list of source files for the executable.
#
# DEPENDS can be used to reference libraries that are defined
# within this project or another cmake project.
#
# EXTERNS can be used to reference libraries that are defined
# outside the project (i.e. by a find package or pkg-config).
#
# DONT_INSTALL can be used to prevent installation of the executable.
# Executables used only for testing should get this flag.
#
macro(ac_add_executable EXECUTABLE_NAME)
    # put arguments into the THIS_EXECUTABLE namespace
    parse_arguments(THIS_EXECUTABLE
        "SOURCES;HEADERS;DEPENDS;EXTERNS;INSTALL_COMPONENT"
        "DONT_INSTALL;NO_REVISION_INFO"
        ${ARGN})

    # bring name into our namespace
    set(THIS_EXECUTABLE_NAME "${EXECUTABLE_NAME}")
    
    # find out how the build info file will be called
    if(NOT THIS_EXECUTABLE_NO_REVISION_INFO)
        _ac_buildinfo_filename(${THIS_EXECUTABLE_NAME} THIS_EXECUTABLE_BUILDINFO_FILE)
    endif(NOT THIS_EXECUTABLE_NO_REVISION_INFO)

    # compute full set of dependencies
    _ac_compute_dependencies(${THIS_EXECUTABLE_NAME} THIS_EXECUTABLE_DEPENDS THIS_EXECUTABLE_EXTERNS)

    # create src symlink in binary dir
    _ac_create_source_symlinks()

    # define the target
    add_executable(
        ${THIS_EXECUTABLE_NAME}
            ${THIS_EXECUTABLE_SOURCES}
            ${THIS_EXECUTABLE_HEADERS}
            ${THIS_EXECUTABLE_RESOURCES}
            ${THIS_EXECUTABLE_BUILDINFO_FILE}
    )

    # add global include and library paths
    _ac_add_global_paths(${THIS_EXECUTABLE_NAME})

    # add paths for external headers and libs
    _ac_add_dependency_paths(
        ${THIS_EXECUTABLE_NAME}
        "${THIS_EXECUTABLE_DEPENDS}" "${THIS_EXECUTABLE_EXTERNS}"
    )

    # update build information
    if(NOT THIS_EXECUTABLE_NO_REVISION_INFO)
        _ac_add_repository_info(
            ${THIS_EXECUTABLE_NAME} "${THIS_EXECUTABLE_BUILDINFO_FILE}"
            EXECUTABLE ${THIS_EXECUTABLE_SOURCES} ${THIS_EXECUTABLE_HEADERS}
        )
    endif(NOT THIS_EXECUTABLE_NO_REVISION_INFO)

    # attach rpath configuration
    _ac_attach_rpath(${THIS_EXECUTABLE_NAME})

    # attach depends and externs
    _ac_attach_depends(${THIS_EXECUTABLE_NAME} "${THIS_EXECUTABLE_DEPENDS}" "${THIS_EXECUTABLE_EXTERNS}")

    # define installation
    if(NOT THIS_EXECUTABLE_DONT_INSTALL)
        # figure out the component to install into
        if(THIS_EXECUTABLE_INSTALL_COMPONENT)
            set(COMPONENT "${THIS_EXECUTABLE_INSTALL_COMPONENT}")
        else(THIS_EXECUTABLE_INSTALL_COMPONENT)
            set(COMPONENT "executable_${THIS_EXECUTABLE_NAME}")
        endif(THIS_EXECUTABLE_INSTALL_COMPONENT)
        # register executable for installation
        install(
            TARGETS ${THIS_EXECUTABLE_NAME}
            RUNTIME
                DESTINATION bin
                COMPONENT ${COMPONENT}
        )

        if(ACMAKE_DEBIAN_PACKAGES)
            string(TOLOWER ${EXECUTABLE_NAME} EXECUTABLE_NAME_LOWER)
            set(PACKAGE ${EXECUTABLE_NAME})
            ac_debian_add_package(
                ${PACKAGE}
                DESCRIPTION "Executable ${LIBRARY_NAME}"
                COMPONENTS ${COMPONENT}
                SHARED_LIBRARY_DEPENDENCIES
            )
        endif(ACMAKE_DEBIAN_PACKAGES)

        # add our target to the current project
        ac_project_add_target(
            EXECUTABLES ${THIS_EXECUTABLE_NAME}
            EXTERNS     ${THIS_EXECUTABLE_EXTERNS}
            DEPENDS     ${THIS_EXECUTABLE_DEPENDS}
        )
    endif(NOT THIS_EXECUTABLE_DONT_INSTALL)
    
endmacro(ac_add_executable)

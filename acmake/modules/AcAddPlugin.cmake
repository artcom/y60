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
# Macro to declare a plugin within the current acmake project.
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

macro(ac_add_plugin PLUGIN_NAME PLUGIN_PATH)
    # put arguments into the THIS_PLUGIN namespace
    parse_arguments(THIS_PLUGIN
        "SOURCES;HEADERS;DEPENDS;EXTERNS"
        "DONT_INSTALL"
        ${ARGN})
    
    # do the same manually for name and path
    set(THIS_PLUGIN_NAME "${PLUGIN_NAME}")
    set(THIS_PLUGIN_PATH "${PLUGIN_PATH}")

    # add aslbase to dependencies
    #   XXX: acmake should not be asl-specific
    list(APPEND THIS_PLUGIN_DEPENDS aslbase)

    # figure out file name for build info
    _ac_buildinfo_filename(${THIS_PLUGIN_NAME} THIS_PLUGIN_BUILDINFO_FILE)

    # create src symlink in binary dir
    _ac_create_source_symlinks()

    # define the target
    add_library(
        ${THIS_PLUGIN_NAME} MODULE
            ${THIS_PLUGIN_SOURCES}
            ${THIS_PLUGIN_HEADERS}
            ${THIS_PLUGIN_BUILDINFO_FILE}
    )

    # add global include and library paths
    _ac_add_global_paths(${THIS_PLUGIN_NAME})

    # declare searchpath for external headers and libs
    _ac_add_dependency_paths(
        ${THIS_PLUGIN_NAME}
        "${THIS_PLUGIN_DEPENDS}" "${THIS_PLUGIN_EXTERNS}"
    )

    # update repository and revision information
    _ac_add_repository_info(
        ${THIS_PLUGIN_NAME} ${THIS_PLUGIN_BUILDINFO_FILE}
        PLUGIN ${THIS_PLUGIN_SOURCES} ${THIS_PLUGIN_HEADERS}
    )

    # attach headers to target
    set_target_properties(
        ${THIS_PLUGIN_NAME} PROPERTIES
            PUBLIC_HEADER "${THIS_PLUGIN_HEADERS}"
    )
    
    # attach rpath configuration
    _ac_attach_rpath(${THIS_PLUGIN_NAME})

    # attach depends and externs
    _ac_attach_depends(${THIS_PLUGIN_NAME} "${THIS_PLUGIN_DEPENDS}" "${THIS_PLUGIN_EXTERNS}")
    
    # define installation
    if(NOT THIS_PLUGIN_DONT_INSTALL)
        install(
            TARGETS ${THIS_PLUGIN_NAME}
            EXPORT ${CMAKE_PROJECT_NAME}
            RUNTIME
                DESTINATION lib/${THIS_PLUGIN_PATH}
            LIBRARY
                DESTINATION lib/${THIS_PLUGIN_PATH}
            PUBLIC_HEADER
                DESTINATION include/${THIS_PLUGIN_PATH}/${THIS_PLUGIN_NAME}
        )
    endif(NOT THIS_PLUGIN_DONT_INSTALL)

    # register plugin with project
    if(NOT THIS_PLUGIN_DONT_INSTALL)
        ac_project_add_target(
            PLUGINS ${THIS_PLUGIN_NAME}
            EXTERNS ${THIS_PLUGIN_EXTERNS}
            DEPENDS ${THIS_PLUGIN_DEPENDS}
        )
    endif(NOT THIS_PLUGIN_DONT_INSTALL)

endmacro(ac_add_plugin)

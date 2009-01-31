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
    parse_arguments(THIS_PLUGIN
        "SOURCES;HEADERS;DEPENDS;EXTERNS;"
        "DONT_INSTALL;"
        ${ARGN})
    
    set(THIS_PLUGIN_NAME "${PLUGIN_NAME}")
    set(THIS_PLUGIN_PATH "${PLUGIN_PATH}")
    
    _ac_declare_searchpath(
        ${THIS_PLUGIN_NAME}
        "${THIS_PLUGIN_DEPENDS}" "${THIS_PLUGIN_EXTERNS}"
    )
    
    # define the target
    add_library(${THIS_PLUGIN_NAME} MODULE ${THIS_PLUGIN_SOURCES}
        ${THIS_PLUGIN_HEADERS})
    
    # link to aslbase because it contains asl::PluginBase
    #   XXX: acmake should not be asl-specific
    target_link_libraries(${THIS_PLUGIN_NAME} aslbase )
    
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
            LIBRARY
                DESTINATION lib/${THIS_PLUGIN_PATH}
            PUBLIC_HEADER
                DESTINATION include/${THIS_PLUGIN_PATH}
        )
    endif(NOT THIS_PLUGIN_DONT_INSTALL)
    
    # XXX: tests?
    
    if(NOT THIS_PLUGIN_DONT_INSTALL)
        ac_project_add_target(PLUGINS ${THIS_PLUGIN_NAME})
    endif(NOT THIS_PLUGIN_DONT_INSTALL)

endmacro(ac_add_plugin)

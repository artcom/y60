#============================================================================
# Copyright (C) 2008, ART+COM Technologies GmbH Berlin
#
# These coded instructions, statements, and computer programs contain
# unpublished proprietary information of ART+COM AG Berlin, and
# are copy protected by law. They may not be disclosed to third parties
# or copied or duplicated in any form, in whole or in part, without the
# specific, prior written permission of ART+COM AG Berlin.
#============================================================================
#
# Macro to declare a plugin within the current acmake project.
#
#============================================================================

macro(ac_add_plugin PLUGIN_NAME PLUGIN_PATH)
    parse_arguments(THIS_PLUGIN
        "SOURCES;HEADERS;DEPENDS;EXTERNS;"
        "DONT_INSTALL;"
        ${ARGN})
    
    set(THIS_PLUGIN_NAME "${PLUGIN_NAME}")
    set(THIS_PLUGIN_PATH "${PLUGIN_PATH}")
    
    _ac_declare_searchpath(${THIS_PLUGIN_EXTERNS})
    
    # define the target
    add_library(${THIS_PLUGIN_NAME} MODULE ${THIS_PLUGIN_SOURCES})
    
    # link to aslbase because it contains asl::PluginBase
    #   XXX: acmake should not be asl-specific
    target_link_libraries(${THIS_PLUGIN_NAME} aslbase )
    
    # attach headers to target
    set_target_properties(
        ${THIS_PLUGIN_NAME} PROPERTIES
            PUBLIC_HEADER "${THIS_PLUGIN_HEADERS}"
    )
    
    # attach depends and externs
    _ac_attach_depends(${THIS_PLUGIN_NAME} ${THIS_PLUGIN_DEPENDS})
    _ac_attach_externs(${THIS_PLUGIN_NAME} ${THIS_PLUGIN_EXTERNS})
    
    # define installation
    if(NOT THIS_PLUGIN_DONT_INSTALL)
        install(
            TARGETS ${THIS_PLUGIN_NAME}
            EXPORT  ${CMAKE_PROJECT_NAME}
            LIBRARY
                DESTINATION lib/${THIS_PLUGIN_PATH}
            PUBLIC_HEADER
                DESTINATION include/${THIS_PLUGIN_PATH}
        )
    endif(NOT THIS_PLUGIN_DONT_INSTALL)
    
    # XXX: tests?
    
endmacro(ac_add_plugin)

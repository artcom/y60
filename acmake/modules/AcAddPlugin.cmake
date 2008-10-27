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

macro(ac_add_plugin PLUGIN_NAME)
  parse_arguments(THIS_PLUGIN
    "SOURCES;HEADERS;DEPENDS;EXTERNS;FRAMEWORK;"
    "DONT_INSTALL;"
    ${ARGN})

  # compute full name
  set(THIS_PLUGIN_NAME "${PLUGIN_NAME}")

  # compute path within project
  set(THIS_PLUGIN_PATH "${CMAKE_PROJECT_NAME}/${PLUGIN_NAME}")

  # define the target
  add_library(${THIS_PLUGIN_NAME} MODULE ${THIS_PLUGIN_SOURCES})
    
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
    set (THIS_PLUGIN_INSTALL_LOCATION lib)
    if(THIS_PLUGIN_FRAMEWORK)
      set (THIS_PLUGIN_INSTALL_LOCATION $THIS_PLUGIN_FRAMEWORK/$THIS_PLUGIN_INSTALL_LOCATION)
    endif(THIS_PLUGIN_FRAMEWORK)
    install(
      TARGETS ${THIS_PLUGIN_NAME}
      LIBRARY
        DESTINATION lib # XXX: depends on container
      PUBLIC_HEADER
        DESTINATION include/${THIS_PLUGIN_PATH}
    )
  endif(NOT THIS_PLUGIN_DONT_INSTALL)

  # XXX: tests?
  
endmacro(ac_add_plugin)

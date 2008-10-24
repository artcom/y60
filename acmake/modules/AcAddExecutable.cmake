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
# Macro to declare an executable within the current acmake project.
#
#============================================================================

macro(ac_add_executable EXECUTABLE_NAME)
  parse_arguments(THIS_EXECUTABLE
    "SOURCES;DEPENDS;EXTERNS;"
    ""
    ${ARGN})

  # compute full name
  set(THIS_EXECUTABLE_NAME "${EXECUTABLE_NAME}")

  # define the target
  add_executable(${THIS_EXECUTABLE_NAME} ${THIS_EXECUTABLE_SOURCES})
    
  # attach depends and externs
  _ac_attach_depends(${THIS_EXECUTABLE_NAME} ${THIS_EXECUTABLE_DEPENDS})
  _ac_attach_externs(${THIS_EXECUTABLE_NAME} ${THIS_EXECUTABLE_EXTERNS})
    
  # define installation
  install(
    TARGETS ${THIS_EXECUTABLE_NAME}
    RUNTIME
      DESTINATION bin
  )

  # XXX: tests?
  
endmacro(ac_add_executable)

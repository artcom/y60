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
# Macro to declare a library within the current acmake project.
#
#============================================================================

macro(ac_add_library LIBRARY_NAME)
  parse_arguments(THIS_LIBRARY
    "SOURCES;HEADERS;DEPENDS;EXTERNS;TESTS;"
    "HEADER_ONLY;"
    ${ARGN})

  # compute full name
  set(THIS_LIBRARY_NAME "${CMAKE_PROJECT_NAME}${LIBRARY_NAME}")

  # compute path within project
  set(THIS_LIBRARY_PATH "${CMAKE_PROJECT_NAME}/${LIBRARY_NAME}")

  if(THIS_LIBRARY_HEADER_ONLY)
    # for a header-only-library

    install(
      FILES ${THIS_LIBRARY_HEADERS}
      DESTINATION include/${THIS_LIBRARY_PATH}
      )

  else(THIS_LIBRARY_HEADER_ONLY)
    # for a full library

    # define the target
    add_library(${THIS_LIBRARY_NAME} SHARED ${THIS_LIBRARY_SOURCES})
    
    # attach headers to target
    set_target_properties(
      ${THIS_LIBRARY_NAME} PROPERTIES
      PUBLIC_HEADER "${THIS_LIBRARY_HEADERS}"
      )

    # attach depends and externs
    _ac_attach_depends(${THIS_LIBRARY_NAME} ${THIS_LIBRARY_DEPENDS})
    _ac_attach_externs(${THIS_LIBRARY_NAME} ${THIS_LIBRARY_EXTERNS})
    
    # define installation
    install(
      TARGETS ${THIS_LIBRARY_NAME}
      LIBRARY
        DESTINATION lib
      PUBLIC_HEADER
        DESTINATION include/${THIS_LIBRARY_PATH}
    )

  endif(THIS_LIBRARY_HEADER_ONLY)

  # create tests
  foreach(TEST ${TESTS})
    # define target
    add_executable(test${TEST} test${TEST}.tst.cpp)

    # care about proper linkage
    if(THIS_LIBRARY_HEADER_ONLY)
      _ac_attach_depends(test${TEST} ${THIS_LIBRARY_DEPENDS})
      _ac_attach_externs(test${TEST} ${THIS_LIBRARY_EXTERNS})
    else(THIS_LIBRARY_HEADER_ONLY)
      target_link_libraries(test${TEST} ${THIS_LIBRARY_NAME})
    endif(THIS_LIBRARY_HEADER_ONLY)

    # tell ctest about it
    add_test(${TEST} test${TEST})
  endforeach(TEST)
  
endmacro(ac_add_library)

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
        "DONT_INSTALL;"
        ${ARGN})

    # compute full name
    set(THIS_EXECUTABLE_NAME "${EXECUTABLE_NAME}")
    
    # declare searchpath for external headers and libs
    _ac_declare_searchpath(${THIS_EXECUTABLE_EXTERNS})

    # define the target
    add_executable(${THIS_EXECUTABLE_NAME} ${THIS_EXECUTABLE_SOURCES})
    
    # attach depends and externs
    _ac_attach_depends(${THIS_EXECUTABLE_NAME} ${THIS_EXECUTABLE_DEPENDS})
    _ac_attach_externs(${THIS_EXECUTABLE_NAME} ${THIS_EXECUTABLE_EXTERNS})

    # define installation
    if(NOT THIS_EXECUTABLE_DONT_INSTALL)
        install(
            TARGETS ${THIS_EXECUTABLE_NAME}
            EXPORT ${CMAKE_PROJECT_NAME}
            RUNTIME
                DESTINATION bin
        )
    endif(NOT THIS_EXECUTABLE_DONT_INSTALL)
    
    # XXX: tests?
    
endmacro(ac_add_executable)

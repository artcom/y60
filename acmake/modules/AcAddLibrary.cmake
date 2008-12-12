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

macro(ac_add_library LIBRARY_NAME LIBRARY_PATH)
    parse_arguments(THIS_LIBRARY
        "SOURCES;HEADERS;DEPENDS;EXTERNS;TESTS;"
        "HEADER_ONLY;DONT_INSTALL;"
        ${ARGN})
    
    set(THIS_LIBRARY_NAME "${LIBRARY_NAME}")
    set(THIS_LIBRARY_PATH "${LIBRARY_PATH}")
    
    if(THIS_LIBRARY_HEADER_ONLY)
        # for a header-only-library
        
        if(NOT THIS_LIBRARY_NO_INSTALL)
            install(
                FILES ${THIS_LIBRARY_HEADERS}
                    DESTINATION include/${THIS_LIBRARY_PATH}
            )
        endif(NOT THIS_LIBRARY_NO_INSTALL)
        
    else(THIS_LIBRARY_HEADER_ONLY)
        # for a full library
        
        # decalre include and library searchpath
        _ac_declare_searchpath(${THIS_LIBRARY_EXTERNS})

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
        if(NOT THIS_LIBRARY_DONT_INSTALL)
            install(
                TARGETS ${THIS_LIBRARY_NAME}
                EXPORT  ${CMAKE_PROJECT_NAME}
                LIBRARY
                    DESTINATION lib
                PUBLIC_HEADER
                    DESTINATION include/${THIS_LIBRARY_PATH}
            )
        endif(NOT THIS_LIBRARY_DONT_INSTALL)
        
    endif(THIS_LIBRARY_HEADER_ONLY)
    
    # create tests
    foreach(TEST ${THIS_LIBRARY_TESTS})
        # define the target
        ac_add_executable(
            test${TEST}
            SOURCES test${TEST}.tst.cpp
            DEPENDS ${THIS_LIBRARY_DEPENDS} ${THIS_LIBRARY_NAME}
            EXTERNS ${THIS_LIBRARY_EXTERNS}
            DONT_INSTALL
        )
        
        # tell ctest about it
        add_test(${TEST} test${TEST})
    endforeach(TEST)
    
endmacro(ac_add_library)

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
# Macro to declare a library within the current acmake project.
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

macro(ac_add_library LIBRARY_NAME LIBRARY_PATH)
    parse_arguments(THIS_LIBRARY
        "SOURCES;HEADERS;DEPENDS;EXTERNS;TESTS;"
        "HEADER_ONLY;DONT_INSTALL;"
        ${ARGN})
    
    set(THIS_LIBRARY_NAME "${LIBRARY_NAME}")
    set(THIS_LIBRARY_PATH "${LIBRARY_PATH}")

    set(THIS_LIBRARY_PATHS_TEMPLATE ${ACMAKE_TEMPLATES_DIR}/AcPaths.h.in)

    configure_file(
         ${THIS_LIBRARY_PATHS_TEMPLATE}
         ${CMAKE_CURRENT_BINARY_DIR}/${THIS_LIBRARY_NAME}_paths.h
         @ONLY
    )
    
    include_directories(${CMAKE_CURRENT_BINARY_DIR})

    if(THIS_LIBRARY_HEADER_ONLY)
        # for a header-only-library
        
        if(NOT THIS_LIBRARY_DONT_INSTALL)
            install(
                FILES ${THIS_LIBRARY_HEADERS}
                    DESTINATION include/${THIS_LIBRARY_PATH}
            )
        endif(NOT THIS_LIBRARY_DONT_INSTALL)
        
    else(THIS_LIBRARY_HEADER_ONLY)
        # for a full library
        
        # decalre include and library searchpath
        _ac_declare_searchpath(${THIS_LIBRARY_EXTERNS})

        # define the target
        add_library(${THIS_LIBRARY_NAME} SHARED ${THIS_LIBRARY_SOURCES}
                ${THIS_LIBRARY_HEADERS})
        
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
                    DESTINATION cmake_parser_hack_foobar
                EXPORT  ${CMAKE_PROJECT_NAME}
                LIBRARY
                    DESTINATION lib
                PUBLIC_HEADER
                    DESTINATION include/${THIS_LIBRARY_PATH}
            )
        endif(NOT THIS_LIBRARY_DONT_INSTALL)
        
    endif(THIS_LIBRARY_HEADER_ONLY)

    # create tests
    if (WIN32)
        set(TEST_NAMESPACE "_${THIS_LIBRARY_NAME}_")
    else (WIN32)
        set(TEST_NAMESPACE "")
    endif(WIN32)
    foreach(TEST ${THIS_LIBRARY_TESTS})
        # define the target
        ac_add_executable(
            test${TEST_NAMESPACE}${TEST}
            SOURCES test${TEST}.tst.cpp
            DEPENDS ${THIS_LIBRARY_DEPENDS} ${THIS_LIBRARY_NAME}
            EXTERNS ${THIS_LIBRARY_EXTERNS}
            DONT_INSTALL
        )
	
        # tell ctest about it
        add_test(${TEST} test${TEST})
    endforeach(TEST)
    
endmacro(ac_add_library)

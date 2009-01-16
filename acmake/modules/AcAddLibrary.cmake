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
                    DESTINATION lib
                EXPORT  ${CMAKE_PROJECT_NAME}
                LIBRARY
                    DESTINATION lib
                PUBLIC_HEADER
                    DESTINATION include/${THIS_LIBRARY_PATH}
            )
        endif(NOT THIS_LIBRARY_DONT_INSTALL)
        
    endif(THIS_LIBRARY_HEADER_ONLY)

    # create tests
    # TODO: - clean up test naming stuff
    #       - maybe add test-namespacing to linux, too?
    set(TEST_NAMESPACE "${THIS_LIBRARY_NAME}")
    foreach(TEST ${THIS_LIBRARY_TESTS})
        if (WIN32)
            set(TEST_EXE_NAME
                    "${TEST_NAMESPACE}_test${TEST}")
            set(TEST_EXE_PATH
                    "${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAMESPACE}_test${TEST}")
        else (WIN32)
            set(TEST_EXE_NAME "test${TEST}" )
            set(TEST_EXE_PATH ${TEST_EXE_NAME} )
        endif (WIN32)
        # define the executable
        ac_add_executable(
            ${TEST_EXE_NAME}
            SOURCES test${TEST}.tst.cpp
            DEPENDS ${THIS_LIBRARY_DEPENDS} ${THIS_LIBRARY_NAME}
            EXTERNS ${THIS_LIBRARY_EXTERNS}
            DONT_INSTALL
        )
	
        # tell ctest about it
        add_test(${THIS_LIBRARY_NAME}_${TEST} ${TEST_EXE_PATH})
    endforeach(TEST)
endmacro(ac_add_library)

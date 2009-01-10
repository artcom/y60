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
# Macro to declare an executable within the current acmake project.
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

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

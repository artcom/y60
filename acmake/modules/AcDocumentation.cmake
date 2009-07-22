# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#
# Copyright (C) 1993-2009, ART+COM AG Berlin, Germany <www.artcom.de>
#
# This file is part of the ART+COM CMake Library (acmake).
#
# It is distributed under the Boost Software License, Version 1.0. 
# (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)             
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#
#
# Documentation generation functions
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

function(ac_add_documentation_target)
# XXX: disabled this because it gets added to ALL_BUILD
#      in visual studio despite not having the ALL flag.
#
#     get_global(AC_DOCUMENTATION_TARGET_DEFINED FLAG)
#     if(NOT FLAG)
#         add_custom_target(
#             docs
#             COMMENT "Generating documentation"
#         )
#         set_global(AC_DOCUMENTATION_TARGET_DEFINED YES)
#     endif(NOT FLAG)
#
#     add_dependencies(docs ${ARGN})
endfunction(ac_add_documentation_target)

# Doxygen - API reference generator

find_package(Doxygen)

function(ac_add_doxygen NAME SOURCE_DIR OUTPUT_DIR DOXYFILE_IN)
    if(DOXYGEN_FOUND)
        make_directory(${OUTPUT_DIR})
        set(AC_DOXYGEN_NAME ${NAME})
        set(AC_DOXYGEN_SOURCE_DIR ${SOURCE_DIR})
        set(AC_DOXYGEN_OUTPUT_DIR ${OUTPUT_DIR})
        ac_configure_file(
            "${DOXYFILE_IN}"
            "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
            "ac_add_doxygen()"
        )
        add_custom_target(
            ${NAME}-doxygen
                "${DOXYGEN_EXECUTABLE}" Doxyfile
                WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                COMMENT "Generating doxygen documentation for ${NAME}"
                VERBATIM
        )
        ac_add_documentation_target(${NAME}-doxygen)
    endif(DOXYGEN_FOUND)
endfunction(ac_add_doxygen PROJECT SOURCE DOXYFILE_IN)

# XSDDOC - Schema reference generator

find_package(Java)

set(AC_XSDDOC_JARS xalan-2.7.0.jar xerces.jar xsddoc.jar)

set(AC_XSDDOC_CLASSPATH "")
foreach(JAR ${AC_XSDDOC_JARS})
    set(AC_XSDDOC_CLASSPATH "${ACMAKE_TOOLS_DIR}/java/${JAR}:${AC_XSDDOC_CLASSPATH}")
endforeach(JAR ${AC_XSDDOC_JARS})

function(ac_add_xsddoc NAME SCHEMA DESTINATION)
    if(JAVA_RUNTIME)
        make_directory(${DESTINATION})
        add_custom_target(
            ${NAME}-xsddoc
                ${JAVA_RUNTIME} -cp ${AC_XSDDOC_CLASSPATH}
                    net.sf.xframe.xsddoc.Main -t ${NAME} -o ${DESTINATION} ${SCHEMA}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Generating xml schema documentation for ${SCHEMA}"
        )
        ac_add_documentation_target(${NAME}-xsddoc)
    endif(JAVA_RUNTIME)
endfunction(ac_add_xsddoc NAME SCHEMA DESTINATION)

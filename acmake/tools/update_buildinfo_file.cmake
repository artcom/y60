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
# subversion repository information utility
# This script inspects the the URL and the revision of the source directory
# and writes a snippet of code to a target file. This file is picked up by the
# build process to provide revision and branch information at runtime.
# The file is only updated when necessary to help with minimal rebuilds.
#
# This script is *not* meant to be included. It should be run as a standalone
# script, like:
# cmake -DSOURCE_DIR=foo -DBINARY_DIR=bar -DTARGET_NAME=libfoobar \
#       -DTARGET_TYPE=library -P AcUpdateSvnRevision.cmake
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

if(NOT TARGET_NAME)
    message(FATAL_ERROR "Variable TARGET_NAME not set")
endif(NOT TARGET_NAME)

if(NOT TARGET_TYPE)
    message(FATAL_ERROR "Variable TARGET_TYPE not set. Should be 'LIBRARY', 'EXECUTABLE' or 'PLUGIN'")
endif(NOT TARGET_TYPE)
string(TOUPPER ${TARGET_TYPE} TARGET_TYPE)
if( NOT TARGET_TYPE MATCHES "LIBRARY|EXECUTABLE|PLUGIN")
    message(FATAL_ERROR "TARGET_TYPE must be 'LIBRARY', 'EXECUTABLE' or 'PLUGIN'")
endif( NOT TARGET_TYPE MATCHES "LIBRARY|EXECUTABLE|PLUGIN")

if(NOT BUILDINFO_FILE)
    message(FATAL_ERROR "Variable BUILDINFO_FILE not set")
endif(NOT BUILDINFO_FILE)

if(NOT SOURCE_DIR)
    message(FATAL_ERROR "Variable SOURCE_DIR not set")
endif(NOT SOURCE_DIR)

if(NOT BUILDINFO_TEMPLATE)
    message(FATAL_ERROR "Variable BUILDINFO_TEMPLATE not set")
endif(NOT BUILDINFO_TEMPLATE)

if(SCM MATCHES "SVN")
    if(NOT SVN_CMD)
        message(FATAL_ERROR "Variable SVN_CMD not set")
    endif(NOT SVN_CMD)
    if(NOT SVNVERSION_CMD)
        message(FATAL_ERROR "Variable SVNVERSION_CMD not set")
    endif(NOT SVNVERSION_CMD)
    execute_process(COMMAND ${SVNVERSION_CMD} ${SOURCE_DIR}
            OUTPUT_VARIABLE ACMAKE_SVN_REVISION
            RESULT_VARIABLE SVNVERSION_EXITCODE
            OUTPUT_STRIP_TRAILING_WHITESPACE)

    if( SVNVERSION_EXITCODE )
        message("Failed to run the 'svnversion' utility.")
        set( ACMAKE_SVN_REVISION "unknown" )
    endif( SVNVERSION_EXITCODE )

    execute_process(COMMAND ${SVN_CMD} info ${SOURCE_DIR}
            OUTPUT_VARIABLE SVN_INFO
            RESULT_VARIABLE SVN_INFO_EXITCODE
            OUTPUT_STRIP_TRAILING_WHITESPACE)

    if( SVN_INFO_EXITCODE )
        message("Failed to run 'svn info'.")
        set( ACMAKE_SVN_URL "unknwon" )
    else( SVN_INFO_EXITCODE )
        string( REGEX REPLACE "\r?\n" ";" SVN_INFO_LINES ${SVN_INFO})
        foreach( line ${SVN_INFO_LINES} )
            if( line MATCHES "^URL: (.*)$" )
                string( REGEX REPLACE "^URL: (.*)$" "\\1" ACMAKE_SVN_URL ${line})
                break()
            endif( line MATCHES "^URL: (.*)$" )
        endforeach( line ${SVN_INFO_LINES} )
        if( NOT ACMAKE_SVN_URL )
            message(SEND_ERROR "Failed to find SVN URL.")
            set(ACMAKE_SVN_URL "unknown")
        endif( NOT ACMAKE_SVN_URL )
    endif( SVN_INFO_EXITCODE )
endif(SCM MATCHES "SVN")

if (SCM)
    set(ACMAKE_HAVE_${SCM}_SCM ON)
endif (SCM)


string(REGEX REPLACE "[-. ]" "_" TARGET_NAME_C ${TARGET_NAME})
string(REGEX REPLACE "[@]" "" TARGET_NAME_C ${TARGET_NAME_C})

set(ACMAKE_GENERATOR "${CMAKE_CURRENT_LIST_FILE}" )
set(ACMAKE_TEMPLATE_FILE "${BUILDINFO_TEMPLATE}" )
# XXX CMake configure_file is smart. it just updates the file
# if the content actually will change. In this case we don't want that.
# Best wy I found so far is to remove the file before updating it :-(
# [DS]
file(REMOVE ${BUILDINFO_FILE})
configure_file( ${BUILDINFO_TEMPLATE}
                ${BUILDINFO_FILE}
                @ONLY)

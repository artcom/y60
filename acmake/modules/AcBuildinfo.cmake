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
# Generation of target-specific build information.
#
# Currently supported information includes:
#  - build date and time
#  - compiler version
#  - revision information (subversion only)
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

option(ACMAKE_BUILDINFO
        "Generate build timestamps and compiler information?" YES)
if(ACMAKE_BUILDINFO)
    if(NOT ACMAKE_BUILDINFO_TEMPLATE)
        message("ACMake currently does not have a default buildinfo template.")
        set(ACMAKE_BUILDINFO OFF)
    endif(NOT ACMAKE_BUILDINFO_TEMPLATE)
    #set( ACMAKE_BUILDINFO_TEMPLATE ${ACMAKE_TEMPLATES_DIR}/buildinfo.cpp.in
    #        CACHE FILEPATH "Buildinfo input template")
    option(ACMAKE_BUILDINFO_SCMDATA
            "Include SCM related data into the build (only svn)?" YES)
    set(ACMAKE_BUILDINFO_FILE_SUFFIX "_buildinfo.cpp")
    if( ACMAKE_BUILDINFO_SCMDATA )
        set(DETECTED_SCMS)
        if( EXISTS ${PROJECT_SOURCE_DIR}/.svn )
            set(SVN_OK YES)
            find_program( SVN svn svn.exe )
            if( NOT SVN)
                message("svn not found. Disabling repository information.")
                set(SVN_OK NO)
            endif( NOT SVN)
            find_program( SVNVERSION svnversion svnversion.exe )
            if( NOT SVNVERSION )
                message("svnversion not found. Disabling repository information.")
                set(SVN_OK NO)
            endif( NOT SVNVERSION )
            if(SVN_OK)
                set(SVN_ARGS -DSVN_CMD=${SVN} -DSVNVERSION_CMD=${SVNVERSION})
                set(SVN_DEPEND_FILES "${CMAKE_CURRENT_SOURCE_DIR}/.svn/entries" )
                list(APPEND DETECTED_SCMS SVN)
            endif(SVN_OK)
        endif( EXISTS ${PROJECT_SOURCE_DIR}/.svn )

        # TODO: check for git

        list(LENGTH DETECTED_SCMS NUM_REPOS)
        if(NUM_REPOS EQUAL 0)
            message("No SCM detected.")
            set(ACMAKE_BUILDINFO_SCMDATA OFF)
        elseif(NUM_REPOS EQUAL 1)
            set(ACMAKE_BUILDINFO_SCM ${DETECTED_SCMS})
        else(NUM_REPOS EQUAL 0)
            list(FIND DETECTED_SCMS "${ACMAKE_BUILDINFO_SCM}" ACMAKE_BUILDINFO_SCM)
            if( NOT ACMAKE_BUILDINFO_SCM)
                message(SEND_ERROR
                        "Schizophrenic working copy. "
                        "Choose a SCM by setting ACMAKE_BUILDINFO_SCM to one of: ${DETECTED_SCMS}")
            endif( NOT ACMAKE_BUILDINFO_SCM)
        endif(NUM_REPOS EQUAL 0)

        set(BUILDINFO_SCM_ARGS ${${ACMAKE_BUILDINFO_SCM}_ARGS})
        set(BUILDINFO_SCM_DEPEND_FILES "${${ACMAKE_BUILDINFO_SCM}_DEPEND_FILES}")
    endif( ACMAKE_BUILDINFO_SCMDATA )

    foreach( CONFIG ${ACMAKE_CONFIGURATION_TYPES} )
        message( ${CONFIG} ) # XXX
    endforeach( CONFIG ${ACMAKE_CONFIGURATION_TYPES} )
endif( ACMAKE_BUILDINFO )

macro(_ac_add_repository_info TARGET_NAME BUILDINFO_FILENAME TARGET_TYPE)
    if(ACMAKE_BUILDINFO)
        set(ALL_DEPEND_FILES ${BUILDINFO_SCM_DEPEND_FILES} ${ARGN})
        add_custom_command(
                OUTPUT ${BUILDINFO_FILENAME}
                COMMAND
                    ${CMAKE_COMMAND}
                          -D "SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
                          -D "TARGET_NAME=${TARGET_NAME}"
                          -D "BUILD_CONFIG=${BUILD_TYPE}"
                          -D "TARGET_TYPE=${TARGET_TYPE}"
                          -D "BUILDINFO_FILE=${BUILDINFO_FILENAME}"
                          -D "BUILDINFO_TEMPLATE=${ACMAKE_BUILDINFO_TEMPLATE}"
                          -D "SCM=${ACMAKE_BUILDINFO_SCM}"
                          ${BUILDINFO_SCM_ARGS}
                          -D "ACMAKE_MODULES_DIR=${ACMAKE_MODULES_DIR}"
                          -P ${ACMAKE_TOOLS_DIR}/update_buildinfo_file.cmake
                 DEPENDS ${ALL_DEPEND_FILES}
                 COMMENT "Updating build information for ${TARGET_NAME}"
        )
        set_source_files_properties(${BUILDINFO_FILENAME} PROPERTIES GENERATED ON)
    endif(ACMAKE_BUILDINFO)
endmacro(_ac_add_repository_info )

macro(_ac_buildinfo_filename TARGET_NAME OUTPUT_VARIABLE)
    if(ACMAKE_BUILDINFO)
        set( ${OUTPUT_VARIABLE}
                "${CMAKE_CURRENT_BINARY_DIR}/${ACMAKE_BINARY_SUBDIR}/${TARGET_NAME}${ACMAKE_BUILDINFO_FILE_SUFFIX}")
    else(ACMAKE_BUILDINFO)
        set(${OUTPUT_VARIABLE})
    endif(ACMAKE_BUILDINFO)
endmacro(_ac_buildinfo_filename)

mark_as_advanced( ACMAKE_BUILDINFO_TEMPLATE SVN SVNVERSION )

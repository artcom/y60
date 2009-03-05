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
# Macros for implementing targets. Used by AcAddExecutable and so forth.
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

option(ACMAKE_DEBUG_LINKAGE "Debug acmake target-specific linkage" NO)
macro(debug_linkage)
    if(ACMAKE_DEBUG_LINKAGE)
        message("LINKAGE: ${ARGN}")
    endif(ACMAKE_DEBUG_LINKAGE)
endmacro(debug_linkage)

option(ACMAKE_DEBUG_SEARCHPATH "Debug acmake target-specific searchpaths" NO)
macro(debug_searchpath)
    if(ACMAKE_DEBUG_SEARCHPATH)
        message("SEARCHPATH: ${ARGN}")
    endif(ACMAKE_DEBUG_SEARCHPATH)
endmacro(debug_searchpath)

option(ACMAKE_DEBUG_DEFINITIONS "Debug acmake target-specific definitions" NO)
macro(debug_definitions)
    if(ACMAKE_DEBUG_DEFINITIONS)
        message("DEFINITIONS: ${ARGN}")
    endif(ACMAKE_DEBUG_DEFINITIONS)
endmacro(debug_definitions)

if(ACMAKE_DEBUG_LINKAGE OR ACMAKE_DEBUG_SEARCHPATH OR ACMAKE_DEBUG_DEFINITIONS)
    set(ACMAKE_DEBUG_TARGET YES)
else(ACMAKE_DEBUG_LINKAGE OR ACMAKE_DEBUG_SEARCHPATH OR ACMAKE_DEBUG_DEFINITIONS)
    set(ACMAKE_DEBUG_TARGET NO)
endif(ACMAKE_DEBUG_LINKAGE OR ACMAKE_DEBUG_SEARCHPATH OR ACMAKE_DEBUG_DEFINITIONS)
macro(debug_target)
    if(ACMAKE_DEBUG_TARGET)
        message("TARGET: ${ARGN}")
    endif(ACMAKE_DEBUG_TARGET)
endmacro(debug_target)

# given set of depends, gives set of libraries to be linked for them
function(_ac_collect_depend_libraries OUT)
    set(LIBRARIES)
    foreach(DEPEND ${ARGN})
        get_target_property(DEPEND_TYPE ${DEPEND} TYPE)
        
        if(DEPEND_TYPE STREQUAL "SHARED_LIBRARY"
                OR DEPEND_TYPE STREQUAL "STATIC_LIBRARY")
            list(APPEND LIBRARIES ${DEPEND})
        endif(DEPEND_TYPE STREQUAL "SHARED_LIBRARY"
                OR DEPEND_TYPE STREQUAL "STATIC_LIBRARY")
    endforeach(DEPEND)
    set(${OUT} "${LIBRARIES}" PARENT_SCOPE)
endfunction(_ac_collect_depend_libraries)

# given set of depends, gives set of externs to be included for them
function(_ac_collect_depend_externs OUT)
    set(EXTERNS)
    foreach(DEPEND ${ARGN})
        get_global(${DEPEND}_PROJECT _PROJECT)
        if(${_PROJECT}_IS_IMPORTED)
            get_global(${_PROJECT}_${DEPEND}_EXTERNS _EXTERNS)
            list(APPEND EXTERNS ${_EXTERNS})
        endif(${_PROJECT}_IS_IMPORTED)
    endforeach(DEPEND)
    set(${OUT} "${EXTERNS}" PARENT_SCOPE)
endfunction(_ac_collect_depend_externs)

# given set of externs, gives set of definitions to be added for them
function(_ac_collect_extern_definitions OUT)
    set(DEFINITIONS)
    foreach(EXTERN ${ARGN})
        if (${EXTERN}_DEFINITIONS)
            list(APPEND DEFINITIONS ${${EXTERN}_DEFINITIONS})
        endif (${EXTERN}_DEFINITIONS)        
    endforeach(EXTERN)
    set(${OUT} "${DEFINITIONS}" PARENT_SCOPE)
endfunction(_ac_collect_extern_definitions)

# given set of externs, gives three sets
# (common, optimized, debug) sets of libraries
# to be linked for them
function(_ac_collect_extern_libraries OUT)
    set(COMMON)
    set(DEBUG)
    set(OPTIMIZED)
    foreach(EXTERN ${ARGN})
        if(EXTERN MATCHES ".*\\.framework/?$")
            # frameworks are trivial
            list(APPEND COMMON ${EXTERN})
        else(EXTERN MATCHES ".*\\.framework/?$")
            # libraries have two cases

            if(${EXTERN}_LIBRARIES_D OR ${EXTERN}_LIBRARY_D)
                # case 1: split debug and optimized

                if(${EXTERN}_LIBRARIES_D)
                    list(APPEND DEBUG ${${EXTERN}_LIBRARIES_D})
                else(${EXTERN}_LIBRARIES_D)
                    list(APPEND DEBUG ${${EXTERN}_LIBRARY_D})
                endif(${EXTERN}_LIBRARIES_D)

                if(${EXTERN}_LIBRARIES)
                    list(APPEND OPTIMIZED ${${EXTERN}_LIBRARIES})
                else(${EXTERN}_LIBRARIES)
                    list(APPEND OPTIMIZED ${${EXTERN}_LIBRARY})
                endif(${EXTERN}_LIBRARIES)

            else(${EXTERN}_LIBRARIES_D OR ${EXTERN}_LIBRARY_D)
                # case 2: only one variant

                if(${EXTERN}_LIBRARIES)
                    list(APPEND COMMON ${${EXTERN}_LIBRARIES})
                else(${EXTERN}_LIBRARIES)
                    list(APPEND COMMON ${${EXTERN}_LIBRARY})
                endif(${EXTERN}_LIBRARIES)

            endif(${EXTERN}_LIBRARIES_D OR ${EXTERN}_LIBRARY_D)
        endif(EXTERN MATCHES ".*\\.framework/?$")
    endforeach(EXTERN)

    set(${OUT}_COMMON    "${COMMON}"    PARENT_SCOPE)
    set(${OUT}_DEBUG     "${DEBUG}"     PARENT_SCOPE)
    set(${OUT}_OPTIMIZED "${OPTIMIZED}" PARENT_SCOPE)
endfunction(_ac_collect_extern_libraries)
    
# attach libraries from DEPENDS and EXTERNS to TARGET
macro(_ac_attach_depends TARGET DEPENDS EXTERNS)

    debug_target("Attaching dependencies for ${TARGET}")

    ### COLLECT EXTERNS

    # depends can pull in externs
    _ac_collect_depend_externs(DEPEND_EXTERNS ${DEPENDS})

    # merge externs
    list(APPEND EXTERNS ${DEPEND_EXTERNS})

    # make externs unique
    if(EXTERNS)
        list(REMOVE_DUPLICATES EXTERNS)
    endif(EXTERNS)


    ### COLLECT DEFINITIONS

    # externs provide definitions
    _ac_collect_extern_definitions(EXTERN_DEFINITIONS ${EXTERNS})

    # alias definitions for consistent naming and make them unique
    set(ALL_DEFINITIONS
        ${EXTERN_DEFINITIONS}
    )

    # transform definitions to normal form
    foreach(DEF ${ALL_DEFINITIONS})
        if(DEF MATCHES "-D.*")
            list(REMOVE_ITEM ALL_DEFINITIONS "${DEF}")
            string(REGEX REPLACE "^-D(.*)$" "\\1" DEF ${DEF})
            list(APPEND ALL_DEFINITIONS ${DEF})
        endif(DEF MATCHES "-D.*")
    endforeach(DEF)

    # make definitions unique
    if(ALL_DEFINITIONS)
        list(REMOVE_DUPLICATES ALL_DEFINITIONS)
    endif(ALL_DEFINITIONS)

    # report definitions
    if(ALL_DEFINITIONS)
        debug_definitions("${TARGET} will be compiled with definitions ${ALL_DEFINITIONS}")
    endif(ALL_DEFINITIONS)


    ### COLLECT LIBRARIES

    # depends are to be linked against
    _ac_collect_depend_libraries(DEPEND_LIBRARIES ${DEPENDS})

    # externs too
    _ac_collect_extern_libraries(EXTERN_LIBRARIES ${EXTERNS})

    # set of all libraries
    set(ALL_LIBRARIES
        ${DEPEND_LIBRARIES}
        ${EXTERN_LIBRARIES_COMMON}
        ${EXTERN_LIBRARIES_DEBUG}
        ${EXTERN_LIBRARIES_OPTIMIZED}
    )
    if(ALL_LIBRARIES)
        list(REMOVE_DUPLICATES ALL_LIBRARIES)
    endif(ALL_LIBRARIES)

    # set of libraries common between build types
    set(COMMON_LIBRARIES
        ${DEPEND_LIBRARIES}
        ${EXTERN_LIBRARIES_COMMON}
    )
    if(COMMON_LIBRARIES)
        list(REMOVE_DUPLICATES COMMON_LIBRARIES)
    endif(COMMON_LIBRARIES)

    # set of libraries only for debug build types
    set(DEBUG_LIBRARIES
        ${EXTERN_LIBRARIES_DEBUG}
    )
    if(DEBUG_LIBRARIES)
        list(REMOVE_DUPLICATES DEBUG_LIBRARIES)
    endif(DEBUG_LIBRARIES)

    # set of libraries only for optimized build types
    set(OPTIMIZED_LIBRARIES
        ${EXTERN_LIBRARIES_OPTIMIZED}
    )
    if(OPTIMIZED_LIBRARIES)
        list(REMOVE_DUPLICATES OPTIMIZED_LIBRARIES)
    endif(OPTIMIZED_LIBRARIES)


    ### DEFINE LINKAGE

    # link common libraries
    target_link_libraries(${TARGET} ${COMMON_LIBRARIES})
    debug_linkage("${TARGET} always links against ${COMMON_LIBRARIES}")

    # link variant-specific libraries one by one
    foreach(LIBRARY ${DEBUG_LIBRARIES})
        target_link_libraries(${TARGET} debug ${LIBRARY})
        debug_linkage("${TARGET}, when compiled debug, links against ${LIBRARY}")
    endforeach(LIBRARY ${DEBUG_LIBRARIES})
    foreach(LIBRARY ${OPTIMIZED_LIBRARIES})
        target_link_libraries(${TARGET} optimized ${LIBRARY})
        debug_linkage("${TARGET}, when compiled optimized, links against ${LIBRARY}")
    endforeach(LIBRARY ${OPTIMIZED_LIBRARIES})


    ### SET RELEVANT TARGET PROPERTIES

    # add libs and defns as target props
    set_target_properties(
        ${TARGET}
        PROPERTIES
            COMPILE_DEFINITIONS "${ALL_DEFINITIONS}"
            LINK_INTERFACE_LIBRARIES_RELEASE        "${COMMON_LIBRARIES};${OPTIMIZED_LIBRARIES}"
            LINK_INTERFACE_LIBRARIES_MINSIZEREL     "${COMMON_LIBRARIES};${OPTIMIZED_LIBRARIES}"
            LINK_INTERFACE_LIBRARIES_RELWITHDEBINFO "${COMMON_LIBRARIES};${OPTIMIZED_LIBRARIES}"
            LINK_INTERFACE_LIBRARIES_DEBUG          "${COMMON_LIBRARIES};${DEBUG_LIBRARIES}"
            # XXX: profile and coverage
    )
endmacro(_ac_attach_depends)

# declare include and library search paths for the given EXTERNS
# on a global scope from this directory downward
# (used by target declarators to globally pull in paths)
macro(_ac_declare_searchpath TARGET DEPENDS EXTERNS)
    _ac_collect_searchpath(${TARGET} LIBRARY INCLUDE "${DEPENDS}" "${EXTERNS}")
    link_directories(${LIBRARY})
    include_directories(${INCLUDE})
    set_global(${TARGET}_INCLUDE_DIRS ${INCLUDE})
    set_global(${TARGET}_LIBRARY_DIRS ${LIBRARY})
endmacro(_ac_declare_searchpath)

# collect include and library search paths for a set
# of external and internal dependencies, returning
# lists of paths in the variables indicated
# by LIBS and INCS
macro(_ac_collect_searchpath TARGET LIBS INCS DEPENDS EXTERNS)
    set(_LIBRARY)
    set(_INCLUDE)

    debug_searchpath("Collecting search paths for ${TARGET}")

    foreach(DEPEND ${DEPENDS})
        get_global(${DEPEND}_LIBRARY_DIRS ${DEPEND}_LIBRARY_DIRS)
        if(${DEPEND}_LIBRARY_DIRS)
            list(APPEND _LIBRARY ${${DEPEND}_LIBRARY_DIRS})
        endif(${DEPEND}_LIBRARY_DIRS)

        get_global(${DEPEND}_INCLUDE_DIRS ${DEPEND}_INCLUDE_DIRS)
        if(${DEPEND}_INCLUDE_DIRS)
            list(APPEND _INCLUDE ${${DEPEND}_INCLUDE_DIRS})
        endif(${DEPEND}_INCLUDE_DIRS)
    endforeach(DEPEND)

    foreach(EXTERN ${EXTERNS})
        if(NOT EXTERN MATCHES ".*\\.framework/?$")
            if(${EXTERN}_LIBRARY_DIRS)
                list(APPEND _LIBRARY ${${EXTERN}_LIBRARY_DIRS})
            else(${EXTERN}_LIBRARY_DIRS)
                if(${EXTERN}_LIBRARY_DIR)
                    list(APPEND _LIBRARY ${${EXTERN}_LIBRARY_DIR})
                endif(${EXTERN}_LIBRARY_DIR)
            endif(${EXTERN}_LIBRARY_DIRS)

            if(${EXTERN}_INCLUDE_DIRS)
                list(APPEND _INCLUDE ${${EXTERN}_INCLUDE_DIRS})
            else(${EXTERN}_INCLUDE_DIRS)
                if(${EXTERN}_INCLUDE_DIR)
                    list(APPEND _INCLUDE ${${EXTERN}_INCLUDE_DIR})
                endif(${EXTERN}_INCLUDE_DIR)
            endif(${EXTERN}_INCLUDE_DIRS)

        endif(NOT EXTERN MATCHES ".*\\.framework/?$")
    endforeach(EXTERN)

    if(_LIBRARY)
        list(REMOVE_DUPLICATES _LIBRARY)
    endif(_LIBRARY)
    if(_INCLUDE)
        list(REMOVE_DUPLICATES _INCLUDE)
    endif(_INCLUDE)

    debug_searchpath("Library: ${_LIBRARY}")
    debug_searchpath("Include: ${_INCLUDE}")

    set(${LIBS} ${_LIBRARY})
    set(${INCS} ${_INCLUDE})
endmacro(_ac_collect_searchpath)

# optionally add an rpath to installed binaries
if(NOT WIN32)
    option(ACMAKE_INSTALL_WITH_RPATH "Should binaries be installed with an rpath?" YES)
endif(NOT WIN32)


macro(_ac_attach_rpath TARGET)
    if(NOT WIN32)
        if(ACMAKE_INSTALL_WITH_RPATH)
            set_target_properties(
                ${TARGET} PROPERTIES
                    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
                    INSTALL_RPATH_USE_LINK_PATH YES
                    INSTALL_NAME_DIR "@executable_path/../lib"
#                    INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
            )
        endif(ACMAKE_INSTALL_WITH_RPATH)
    endif(NOT WIN32)
endmacro(_ac_attach_rpath)

if(NOT WIN32)
    option(ACMAKE_SYMLINK_SOURCES_TO_BUILDTREE "Create symlinks <bindir>/src -> <srcdir>" YES)
endif(NOT WIN32)

macro(_ac_create_source_symlinks)
    if( NOT WIN32 AND ACMAKE_SYMLINK_SOURCES_TO_BUILDTREE)
        if( NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/src)
            file(RELATIVE_PATH REL ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
                execute_process(COMMAND cmake -E create_symlink ${REL} src
                                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
        endif( NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/src)
    endif( NOT WIN32 AND ACMAKE_SYMLINK_SOURCES_TO_BUILDTREE)
endmacro(_ac_create_source_symlinks)

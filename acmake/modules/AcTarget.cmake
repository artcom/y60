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

option(ACMAKE_DEBUG_LINKAGE "Debug acmake linkage mechanism" NO)
macro(debug_linkage)
    if(ACMAKE_DEBUG_LINKAGE)
        message("LINKAGE: ${ARGN}")
    endif(ACMAKE_DEBUG_LINKAGE)
endmacro(debug_linkage)

option(ACMAKE_DEBUG_SEARCHPATH "Debug acmake include and library search path mechanism" NO)
macro(debug_searchpath)
    if(ACMAKE_DEBUG_SEARCHPATH)
        message("SEARCHPATH: ${ARGN}")
    endif(ACMAKE_DEBUG_SEARCHPATH)
endmacro(debug_searchpath)

# attach libraries from DEPENDS and EXTERNS to TARGET
macro(_ac_attach_depends TARGET DEPENDS EXTERNS)

    debug_linkage("Collecting depends for ${TARGET}")

    # collect libraries from depends
    set(DEPEND_LIBRARIES)
    foreach(DEPEND ${DEPENDS})
        get_target_property(DEPEND_TYPE ${DEPEND} TYPE)

        if("XXX${DEPEND_TYPE}XXX" STREQUAL "XXXSHARED_LIBRARYXXX"
                OR "XXX${DEPEND_TYPE}XXX" STREQUAL "XXXSTATIC_LIBRARYXXX")
            list(APPEND DEPEND_LIBRARIES ${DEPEND})
        endif("XXX${DEPEND_TYPE}XXX" STREQUAL "XXXSHARED_LIBRARYXXX"
            OR "XXX${DEPEND_TYPE}XXX" STREQUAL "XXXSTATIC_LIBRARYXXX")
    endforeach(DEPEND)

    # collect libraries from externs
    set(EXTERN_DEFINITIONS)
    set(EXTERN_LIBRARIES_GENERAL)
    set(EXTERN_LIBRARIES_DEBUG)
    set(EXTERN_LIBRARIES_OPTIMIZED)
    foreach(EXTERN ${EXTERNS})
        if(EXTERN MATCHES ".*\\.framework/?$")
            list(APPEND EXTERN_LIBRARIES_GENERAL ${EXTERN})
        else(EXTERN MATCHES ".*\\.framework/?$")
            if(${EXTERN}_LIBRARIES_D)
                list(APPEND EXTERN_LIBRARIES_DEBUG     ${${EXTERN}_LIBRARIES_D})
                list(APPEND EXTERN_LIBRARIES_OPTIMIZED ${${EXTERN}_LIBRARIES})
            else(${EXTERN}_LIBRARIES_D)
                list(APPEND EXTERN_LIBRARIES_GENERAL ${${EXTERN}_LIBRARIES})
            endif(${EXTERN}_LIBRARIES_D)
        endif(EXTERN MATCHES ".*\\.framework/?$")
        if ( ${EXTERN}_DEFINITIONS )
            list(APPEND EXTERN_DEFINITIONS ${${EXTERN}_DEFINITIONS})
            set_target_properties(
                ${TARGET} 
                    PROPERTIES
                    COMPILE_DEFINITIONS ${EXTERN_DEFINITIONS}
            )
        endif ( ${EXTERN}_DEFINITIONS )
    endforeach(EXTERN)
    
    # collect libraries into various subsets
    set(ALL_DEFINITIONS
        ${EXTERN_DEFINITIONS}
    )
    if(ALL_DEFINITIONS)
        list(REMOVE_DUPLICATES ALL_DEFINITIONS)
    endif(ALL_DEFINITIONS)
    set(ALL_LIBRARIES
        ${DEPEND_LIBRARIES}
        ${EXTERN_LIBRARIES_GENERAL}
        ${EXTERN_LIBRARIES_DEBUG}
        ${EXTERN_LIBRARIES_OPTIMIZED}
    )
    if(ALL_LIBRARIES)
        list(REMOVE_DUPLICATES ALL_LIBRARIES)
    endif(ALL_LIBRARIES)
    set(GENERAL_LIBRARIES
        ${DEPEND_LIBRARIES}
        ${EXTERN_LIBRARIES_GENERAL}
    )
    if(GENERAL_LIBRARIES)
        list(REMOVE_DUPLICATES GENERAL_LIBRARIES)
    endif(GENERAL_LIBRARIES)
    set(DEBUG_LIBRARIES
        ${EXTERN_LIBRARIES_DEBUG}
    )
    if(DEBUG_LIBRARIES)
        list(REMOVE_DUPLICATES DEBUG_LIBRARIES)
    endif(DEBUG_LIBRARIES)
    set(OPTIMIZED_LIBRARIES
        ${EXTERN_LIBRARIES_OPTIMIZED}
    )
    if(OPTIMIZED_LIBRARIES)
        list(REMOVE_DUPLICATES OPTIMIZED_LIBRARIES)
    endif(OPTIMIZED_LIBRARIES)

    # define linkage
    target_link_libraries(${TARGET} ${GENERAL_LIBRARIES})
    debug_linkage("${TARGET} always links against ${GENERAL_LIBRARIES}")
    foreach(LIBRARY ${DEBUG_LIBRARIES})
        target_link_libraries(${TARGET} debug ${LIBRARY})
        debug_linkage("${TARGET} links against ${LIBRARY} when compiled debug")
    endforeach(LIBRARY ${DEBUG_LIBRARIES})
    foreach(LIBRARY ${OPTIMIZED_LIBRARIES})
        target_link_libraries(${TARGET} optimized ${LIBRARY})
        debug_linkage("${TARGET} links against ${LIBRARY} when compiled optimized")
    endforeach(LIBRARY ${OPTIMIZED_LIBRARIES})

    # declare all libraries as transient link deps
    # and make extern definitions available to source files
    set_target_properties(
        ${TARGET}
        PROPERTIES
            LINK_INTERFACE_LIBRARIES "${ALL_LIBRARIES}"
            COMPILE_DEFINITIONS "${ALL_DEFINITIONS}"
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

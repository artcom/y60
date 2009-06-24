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
# Common infrastructure for implementing build targets.
#
# This contains the mechanisms for:
#  - linkage determination
#  - search path determination
#  - definition determination
#  - dependency propagation
#  - rpath handling
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

# define some debug hooks for tracing
ac_define_debug_channel(target      "Debug target analysis")
ac_define_debug_channel(linkage     "Debug target linkage")
ac_define_debug_channel(searchpaths "Debug target searchpaths")
ac_define_debug_channel(definitions "Debug target definitions")

# utility function for extending FLAGS-like target properties
function(_ac_prepend_target_flag TARGET PROPERTY VALUE)
    get_property(FLAGS TARGET ${TARGET} PROPERTY ${PROPERTY})
    set_property(TARGET ${TARGET} PROPERTY ${PROPERTY} "${VALUE} ${FLAGS}")
endfunction(_ac_prepend_target_flag TARGET PROPERTY VALUE)

# add include path to a target for its own use
# PROPAGATE indicates if this path should be used by dependents
function(_ac_add_include_path TARGET PATH PROPAGATE)
    ac_debug_searchpaths("${TARGET} gets include path ${PATH}")
    if(MSVC)
        _ac_prepend_target_flag(${TARGET} COMPILE_FLAGS "/I \"${PATH}\"")
    else(MSVC)
        _ac_prepend_target_flag(${TARGET} COMPILE_FLAGS "-I\"${PATH}\"")
    endif(MSVC)
    if(PROPAGATE)
        prepend_global(${TARGET}_INCLUDE_DIRS "${PATH}")
    endif(PROPAGATE)
endfunction(_ac_add_include_path TARGET PATH)

# add library path to a target for its own use
# PROPAGATE indicates if this path should be used by dependents
function(_ac_add_library_path TARGET PATH PROPAGATE)
    ac_debug_searchpaths("${TARGET} gets library path ${PATH}")
    if(MSVC)
        _ac_prepend_target_flag(${TARGET} LINK_FLAGS "/LIBPATH:\"${PATH}\"")
    else(MSVC)
        _ac_prepend_target_flag(${TARGET} LINK_FLAGS "-L\"${PATH}\"")
    endif(MSVC)
    if(PROPAGATE)
        prepend_global(${TARGET}_LIBRARY_DIRS "${PATH}")
    endif(PROPAGATE)
endfunction(_ac_add_library_path TARGET PATH)

# add build-directory-global include and library paths to target
function(_ac_add_global_paths TARGET)
    foreach(INCLUDE_DIR ${ACMAKE_GLOBAL_INCLUDE_DIRS})
        _ac_add_include_path(${TARGET} "${INCLUDE_DIR}" NO)
    endforeach(INCLUDE_DIR ${ACMAKE_GLOBAL_INCLUDE_DIRS})
    foreach(LIBRARY_DIR ${ACMAKE_GLOBAL_LIBRARY_DIRS})
        _ac_add_library_path(${TARGET} "${LIBRARY_DIR}" NO)
    endforeach(LIBRARY_DIR ${ACMAKE_GLOBAL_LIBRARY_DIRS})
endfunction(_ac_add_global_paths TARGET)

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

# given set of depends, gives set of externs to be used for them to work
function(_ac_collect_depend_externs OUT)
    set(EXTERNS)
    foreach(DEPEND ${ARGN})
        get_global(${DEPEND}_PROJECT _PROJECT)
        # we only concern ourselves with depends that come
        # from acmake projects as only those will export
        # the necessary information
        if(_PROJECT)
            # ignore externs for targets that are defined
            # within the current project as those will
            # have specified the necessary information
            # as part of their declaration
            if(NOT ACMAKE_CURRENT_PROJECT STREQUAL ${_PROJECT})
                list(APPEND EXTERNS ${_PROJECT})
            endif(NOT ACMAKE_CURRENT_PROJECT STREQUAL ${_PROJECT})
            get_global(${DEPEND}_EXTERNS _EXTERNS)
            list(APPEND EXTERNS ${_EXTERNS})
        endif(_PROJECT)
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

# given set of externs, gives three sets (common, optimized, debug)
# of libraries to be linked when using them
#
# returns results in ${OUT}_COMMON, ${OUT}_OPTIMIZED and ${OUT}_DEBUG
function(_ac_collect_extern_libraries OUT)
    set(COMMON)
    set(DEBUG)
    set(OPTIMIZED)
    foreach(EXTERN ${ARGN})
        if(${EXTERN}_IS_PROJECT)
            # if the extern is a project, its libs
            # do not need to be collected as they
            # are registered as targets and will
            # be handled by automatic transient linkage
        elseif(EXTERN MATCHES ".*\\.framework/?$")
            # frameworks are trivial
            list(APPEND COMMON ${EXTERN})
        else(${EXTERN}_IS_PROJECT)
            # normal libraries have two cases

            if(${EXTERN}_LIBRARIES_D OR ${EXTERN}_LIBRARY_D OR ${EXTERN}_LIBRARIES_C OR ${EXTERN}_LIBRARY_C)
                # case 1: split debug, optimized and common given by package

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

                if(${EXTERN}_LIBRARIES_C)
                    list(APPEND COMMON ${${EXTERN}_LIBRARIES_C})
                else(${EXTERN}_LIBRARIES_C)
                    list(APPEND COMMON ${${EXTERN}_LIBRARY_C})
                endif(${EXTERN}_LIBRARIES_C)

            else(${EXTERN}_LIBRARIES_D OR ${EXTERN}_LIBRARY_D OR ${EXTERN}_LIBRARIES_C OR ${EXTERN}_LIBRARY_C)
                # case 2: only one variant that will go into the common set

                if(${EXTERN}_LIBRARIES)
                    list(APPEND COMMON ${${EXTERN}_LIBRARIES})
                else(${EXTERN}_LIBRARIES)
                    list(APPEND COMMON ${${EXTERN}_LIBRARY})
                endif(${EXTERN}_LIBRARIES)

            endif(${EXTERN}_LIBRARIES_D OR ${EXTERN}_LIBRARY_D OR ${EXTERN}_LIBRARIES_C OR ${EXTERN}_LIBRARY_C)
        endif(${EXTERN}_IS_PROJECT)
    endforeach(EXTERN)

    # set result variables
    set(${OUT}_COMMON    "${COMMON}"    PARENT_SCOPE)
    set(${OUT}_DEBUG     "${DEBUG}"     PARENT_SCOPE)
    set(${OUT}_OPTIMIZED "${OPTIMIZED}" PARENT_SCOPE)
endfunction(_ac_collect_extern_libraries)

# do dependency expansion (of DEPEND and EXTERN dependencies)
# for the given target, reading and destructively modifying
# the given variables, which shall contain the set of
# externs and depends that the target requires
function(_ac_compute_dependencies TARGET DEPENDS_VAR EXTERNS_VAR)
    set(DEPENDS ${${DEPENDS_VAR}})
    set(EXTERNS ${${EXTERNS_VAR}})


    ### COLLECT DEPENDS

    foreach(DEPEND ${DEPENDS})
        get_global(${DEPEND}_DEPENDS DEPEND_DEPENDS)
        list(APPEND DEPENDS ${DEPEND_DEPENDS})
    endforeach(DEPEND ${DEPENDS})

    if(DEPENDS)
        list(REMOVE_DUPLICATES DEPENDS)
    endif(DEPENDS)


    ### COLLECT EXTERNS

    # depends can pull in externs
    _ac_collect_depend_externs(DEPEND_EXTERNS ${DEPENDS})

    # merge externs
    list(APPEND EXTERNS ${DEPEND_EXTERNS})
    if(EXTERNS)
        list(REMOVE_DUPLICATES EXTERNS)
    endif(EXTERNS)


    # set result variables
    set(${DEPENDS_VAR} ${DEPENDS} PARENT_SCOPE)
    set(${EXTERNS_VAR} ${EXTERNS} PARENT_SCOPE)
endfunction(_ac_compute_dependencies)

# attach libraries from DEPENDS and EXTERNS to TARGET
#
# this will collect definitions and linkable libraries,
# attaching them to the target for use in compilation and linkage
macro(_ac_attach_depends TARGET DEPENDS EXTERNS)

    ac_debug_target("attaching dependencies for ${TARGET}")

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
        ac_debug_definitions("${TARGET} will be compiled with definitions ${ALL_DEFINITIONS}")
    endif(ALL_DEFINITIONS)


    ### COLLECT LIBRARIES

    # depends are to be linked against
    _ac_collect_depend_libraries(DEPEND_LIBRARIES ${DEPENDS})

    # externs too
    _ac_collect_extern_libraries(EXTERN_LIBRARIES ${EXTERNS})

    # XXX Hack to filter "optimzed" and "debug" keywords left by FindBoost
    #     Probably something Ingo wants to take a look at. [DS]
    string(REPLACE "optimized" "" tmp "${EXTERN_LIBRARIES_COMMON}")
    set(EXTERN_LIBRARIES_COMMON ${tmp})
    string(REPLACE "debug" "" tmp "${EXTERN_LIBRARIES_COMMON}")
    set(EXTERN_LIBRARIES_COMMON ${tmp})
    string(REPLACE "optimized" "" tmp "${EXTERN_LIBRARIES_DEBUG}")
    set(EXTERN_LIBRARIES_DEBUG ${tmp})
    string(REPLACE "debug" "" tmp "${EXTERN_LIBRARIES_DEBUG}")
    set(EXTERN_LIBRARIES_DEBUG ${tmp})
    string(REPLACE "optimized" "" tmp "${EXTERN_LIBRARIES_OPTIMIZED}")
    set(EXTERN_LIBRARIES_OPTIMIZED ${tmp})
    string(REPLACE "debug" "" tmp "${EXTERN_LIBRARIES_OPTIMIZED}")
    set(EXTERN_LIBRARIES_OPTIMIZED ${tmp})


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
    ac_debug_linkage("${TARGET} always links against ${COMMON_LIBRARIES}")

    # link variant-specific libraries
    ac_debug_linkage("${TARGET}, when compiled debug, links against ${DEBUG_LIBRARIES}")
    foreach(LIBRARY ${DEBUG_LIBRARIES})
        target_link_libraries(${TARGET} debug ${LIBRARY})
    endforeach(LIBRARY ${DEBUG_LIBRARIES})
    ac_debug_linkage("${TARGET}, when compiled optimized, links against ${OPTIMIZED_LIBRARIES}")
    foreach(LIBRARY ${OPTIMIZED_LIBRARIES})
        target_link_libraries(${TARGET} optimized ${LIBRARY})
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

# add include and library search paths for the given
# EXTERNS and DEPENDS to the given TARGET
macro(_ac_add_dependency_paths TARGET DEPENDS EXTERNS)
    _ac_collect_dependency_paths(${TARGET} LIBRARY_DIRS INCLUDE_DIRS "${DEPENDS}" "${EXTERNS}")
    
    foreach(DIR ${LIBRARY_DIRS})
        _ac_add_library_path(${TARGET} "${DIR}" YES)
    endforeach(DIR)

    foreach(DIR ${INCLUDE_DIRS})
        _ac_add_include_path(${TARGET} "${DIR}" YES)
    endforeach(DIR)
endmacro(_ac_add_dependency_paths)

# collect include and library search paths for a set
# of external and internal dependencies, returning
# lists of paths in the variables indicated
# by LIBS and INCS
macro(_ac_collect_dependency_paths TARGET LIBS INCS DEPENDS EXTERNS)
    set(_EXTERNS ${EXTERNS})
    set(_LIBRARY)
    set(_INCLUDE)

    ac_debug_searchpaths("collecting paths for dependencies of ${TARGET}")

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

    if(_EXTERNS)
        list(REMOVE_DUPLICATES _EXTERNS)
    endif(_EXTERNS)

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

    set(${LIBS} ${_LIBRARY})
    set(${INCS} ${_INCLUDE})
endmacro(_ac_collect_dependency_paths)

# optionally add an rpath to installed binaries
# (configurable to enable movable builds)
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

# optionally create a symlink from the binary directory to the source directory
# (this feature is for development comfort in unixoid systems)
if(NOT WIN32)
    option(ACMAKE_SYMLINK_SOURCES_TO_BUILDTREE "Create symlinks <bindir>/src -> <srcdir>" YES)
endif(NOT WIN32)

macro(_ac_create_source_symlinks)
    if(NOT WIN32 AND ACMAKE_SYMLINK_SOURCES_TO_BUILDTREE)
        if(NOT "${CMAKE_CURRENT_BINARY_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
            if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/src)
                file(RELATIVE_PATH REL ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
                execute_process(COMMAND cmake -E create_symlink ${REL} src
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
            endif(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/src)
        endif(NOT "${CMAKE_CURRENT_BINARY_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    endif(NOT WIN32 AND ACMAKE_SYMLINK_SOURCES_TO_BUILDTREE)
endmacro(_ac_create_source_symlinks)

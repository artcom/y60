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
# Macro to declare a library in the current acmake project
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

# Define a library in the current ACMake project.
#
# ac_add_library(
#    name path
#    HEADERS headerfile ...
#    [ SOURCES sourcefile ... ]
#    [ DEPENDS integrated_library ... ]
#    [ EXTERNS imported_package ... ]
#    [ TESTS   testname ... ]
#    [ DONT_INSTALL ]
#    [ STATIC ]
# )
#
# The NAME is used in project exports and for the target name.
#
# The PATH defines where includes are installed within the installed include
# tree, which should be the path that will be used for actually including
# the files in a source file (i.e. "#include <foo/bar/bla.h>" => "foo/bar").
#
# HEADERS is a list of all public header files for the library.
#
# SOURCES is a list of source files for this library. If empty or not present
# all the library is treated as a header-only library
#
# DEPENDS can be used to reference libraries that are defined
# within this project or another cmake project.
#
# EXTERNS can be used to reference libraries that are defined
# outside the project (i.e. by a find package or pkg-config).
#
# TESTS is a list of tests using a certain old A+C naming scheme.
#
# DONT_INSTALL can be used to prevent installation of this library.
# Libraries used only for testing should get this flag.
#
# STATIC libraries are supported too.
#
macro(ac_add_library LIBRARY_NAME LIBRARY_PATH)
    # put arguments into the THIS_LIBRARY namespace
    parse_arguments(THIS_LIBRARY
        "SOURCES;HEADERS;DEPENDS;EXTERNS;TESTS;RUNTIME_INSTALL_COMPONENT;DEVELOPMENT_INSTALL_COMPONENT;SOVERSION;VERSION"
        "DONT_INSTALL;STATIC"
        ${ARGN}
    )
    
    # do the same manually for name and path
    set(THIS_LIBRARY_NAME "${LIBRARY_NAME}")
    set(THIS_LIBRARY_PATH "${LIBRARY_PATH}")

    # determine version information
    if(NOT THIS_LIBRARY_SOVERSION)
        message("Library ${LIBRARY_NAME} does not specify a SOVERSION. Defaulting to 0.")
        set(THIS_LIBRARY_SOVERSION 0)
    endif(NOT THIS_LIBRARY_SOVERSION)
    if(NOT THIS_LIBRARY_VERSION)
        message("Library ${LIBRARY_NAME} does not specify a VERSION. Defaulting to 1.0.0.")
        set(THIS_LIBRARY_VERSION 1.0.0)
    endif(NOT THIS_LIBRARY_VERSION)

    # generate a header describing our binary, source and install locations
    set(THIS_LIBRARY_PATHS_TEMPLATE ${ACMAKE_TEMPLATES_DIR}/AcPaths.h.in)
    ac_configure_file(
         ${THIS_LIBRARY_PATHS_TEMPLATE}
         ${CMAKE_CURRENT_BINARY_DIR}/${ACMAKE_BINARY_SUBDIR}/acmake/${THIS_LIBRARY_NAME}_paths.h
         "ac_add_library()"
    )

    # compute full set of dependencies
    _ac_compute_dependencies(${THIS_LIBRARY_NAME} THIS_LIBRARY_DEPENDS THIS_LIBRARY_EXTERNS)

    # create src symlink in binary dir
    _ac_create_source_symlinks()

    if(NOT THIS_LIBRARY_SOURCES)
        set(THIS_LIBRARY_HEADER_ONLY ON)
        # CMake currently does not support header only libraries. To work around
        # this limitation we create a dummy source file while configuring and
        # treat the library as a full library afterwards.
        set(_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${ACMAKE_BINARY_SUBDIR})
        set(_OUT_NAME cmake_header_only_workaround.cpp)
        set(_OUT_FILE "${_OUT_DIR}/${_OUT_NAME}")
        # TODO: check if we need a symbol at all. If not just use touch.
        add_custom_command(
                OUTPUT ${_OUT_FILE} 
                COMMAND
                    ${CMAKE_COMMAND}
                        -E copy_if_different ${ACMAKE_TEMPLATES_DIR}/AcCMakeHeaderOnlyWorkaround.cpp
                                             ${_OUT_FILE}
                DEPENDS ${THIS_LIBRARY_HEADERS}
                COMMENT "Generating header-only stub for library ${THIS_LIBRARY_NAME}"
        )
        set(THIS_LIBRARY_SOURCES ${_OUT_FILE})
        set(THIS_LIBRARY_BUILDINFO_FILE)
    else(NOT THIS_LIBRARY_SOURCES)
        set(THIS_LIBRARY_HEADER_ONLY OFF)
        # figure out file name for build info
        _ac_buildinfo_filename("${THIS_LIBRARY_NAME}" THIS_LIBRARY_BUILDINFO_FILE)
    endif(NOT THIS_LIBRARY_SOURCES)
        
    if(THIS_LIBRARY_STATIC)
        set(THIS_LIBRARY_STYLE STATIC)
    else(THIS_LIBRARY_STATIC)
        set(THIS_LIBRARY_STYLE SHARED)
    endif(THIS_LIBRARY_STATIC)
    # define the library target
    add_library(
        ${THIS_LIBRARY_NAME} ${THIS_LIBRARY_STYLE}
            ${THIS_LIBRARY_SOURCES}
            ${THIS_LIBRARY_HEADERS}
            ${THIS_LIBRARY_BUILDINFO_FILE}
    )

    # add global include and library paths
    _ac_add_global_paths(${THIS_LIBRARY_NAME})

    # declare include and library searchpath
    _ac_add_dependency_paths(
        ${THIS_LIBRARY_NAME}
        "${THIS_LIBRARY_DEPENDS}" "${THIS_LIBRARY_EXTERNS}"
    )

    # add path to generated headers (XXX: only done for libs right now)
    _ac_add_include_path(${THIS_LIBRARY_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${ACMAKE_BINARY_SUBDIR} NO)

    if(NOT THIS_LIBRARY_HEADER_ONLY)
        # update repository and revision information
        _ac_add_repository_info(
                ${THIS_LIBRARY_NAME} "${THIS_LIBRARY_BUILDINFO_FILE}"
                LIBRARY ${THIS_LIBRARY_SOURCES} ${THIS_LIBRARY_HEADERS}
        )
    endif(NOT THIS_LIBRARY_HEADER_ONLY)

    # attach headers to target
    set_target_properties(
        ${THIS_LIBRARY_NAME} PROPERTIES
            PUBLIC_HEADER "${THIS_LIBRARY_HEADERS}"
    )

    # attach rpath configuration
    _ac_attach_rpath(${THIS_LIBRARY_NAME})
    
    # attach depends and externs
    _ac_attach_depends(${THIS_LIBRARY_NAME} "${THIS_LIBRARY_DEPENDS}" "${THIS_LIBRARY_EXTERNS}")
    
    # apply version properties
    set_target_properties(
        ${THIS_LIBRARY_NAME} PROPERTIES
        VERSION ${THIS_LIBRARY_VERSION}
        SOVERSION ${THIS_LIBRARY_SOVERSION}
    )

    # define installation
    if(NOT THIS_LIBRARY_DONT_INSTALL)
        # figure out components to install into
        set(COMPONENT_RUNTIME)
        set(COMPONENT_DEVELOPMENT)
        if(ACMAKE_CURRENT_PROJECT)
            if(THIS_LIBRARY_RUNTIME_INSTALL_COMPONENT)
                set(COMPONENT_RUNTIME     "${THIS_LIBRARY_RUNTIME_INSTALL_COMPONENT}")
            else(THIS_LIBRARY_RUNTIME_INSTALL_COMPONENT)
                set(COMPONENT_RUNTIME     "library_${THIS_LIBRARY_NAME}_runtime")
            endif(THIS_LIBRARY_RUNTIME_INSTALL_COMPONENT)
            if(THIS_LIBRARY_DEVELOPMENT_INSTALL_COMPONENT)
                set(COMPONENT_DEVELOPMENT "${THIS_LIBRARY_DEVELOPMENT_INSTALL_COMPONENT}")
            else(THIS_LIBRARY_DEVELOPMENT_INSTALL_COMPONENT)
                set(COMPONENT_DEVELOPMENT "library_${THIS_LIBRARY_NAME}_development")
            endif(THIS_LIBRARY_DEVELOPMENT_INSTALL_COMPONENT)
        endif(ACMAKE_CURRENT_PROJECT)
        # register target for installation
        install(
            TARGETS ${THIS_LIBRARY_NAME}
            RUNTIME
                DESTINATION bin
                COMPONENT ${COMPONENT_RUNTIME}
            LIBRARY
                DESTINATION lib
                COMPONENT ${COMPONENT_RUNTIME}
            ARCHIVE
                DESTINATION lib
                COMPONENT ${COMPONENT_DEVELOPMENT}
            PUBLIC_HEADER
                DESTINATION include/${THIS_LIBRARY_PATH}
                COMPONENT ${COMPONENT_DEVELOPMENT}
        )

        if(ACMAKE_DEBIAN)
            string(TOLOWER ${LIBRARY_NAME} LIBRARY_NAME_LOWER)
            set(PACKAGE_RUNTIME     lib${LIBRARY_NAME_LOWER}${THIS_LIBRARY_VERSION}-${THIS_LIBRARY_SOVERSION})
            set(PACKAGE_DEVELOPMENT lib${LIBRARY_NAME_LOWER}${THIS_LIBRARY_VERSION}-dev)
            ac_debian_add_package(
                ${PACKAGE_RUNTIME}
                DESCRIPTION "Library ${LIBRARY_NAME} runtime"
                COMPONENTS ${COMPONENT_RUNTIME}
                SHARED_LIBRARY_DEPENDENCIES
            )
            ac_debian_add_package(
                ${PACKAGE_DEVELOPMENT}
                DESCRIPTION "Library ${LIBRARY_NAME} development"
                COMPONENTS ${COMPONENT_DEVELOPMENT}
                DEPENDS lib${LIBRARY_NAME}${THIS_LIBRARY_VERSION}
                RECOMMENDS acmake
            )
        endif(ACMAKE_DEBIAN)
    endif(NOT THIS_LIBRARY_DONT_INSTALL)

    # create tests
    # TODO: clean up test naming
    # XXX: This is goddamn ugly.
    #      Maybe, we should replace it with a separate subsystem.
    if(ACMAKE_BUILD_TESTS)
        set(TEST_NAMESPACE "${THIS_LIBRARY_NAME}")
        foreach(TEST ${THIS_LIBRARY_TESTS})
            set(TEST_NAME "${TEST_NAMESPACE}_test${TEST}")
            
            # header only libraries do not require the lib which only
            # exists as a workaround
            if(THIS_LIBRARY_HEADER_ONLY)
                set(THIS_LIBRARY_MAYBE_LINK_SELF)
            else(THIS_LIBRARY_HEADER_ONLY)
                set(THIS_LIBRARY_MAYBE_LINK_SELF ${THIS_LIBRARY_NAME})
            endif(THIS_LIBRARY_HEADER_ONLY)
            # define the executable
            ac_add_executable(
                ${TEST_NAME}
                SOURCES test${TEST}.tst.cpp
                DEPENDS ${THIS_LIBRARY_DEPENDS} ${THIS_LIBRARY_MAYBE_LINK_SELF} # XXX: asl-specifics
                EXTERNS ${THIS_LIBRARY_EXTERNS}
                DONT_INSTALL
                NO_REVISION_INFO
            )
        
            # add path to generated headers from our parent library
            # XXX: maybe remove when executable has own conf header?
            _ac_add_include_path(${TEST_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${ACMAKE_BINARY_SUBDIR} NO)
        
            # XXX: We know how to do better here! We should use
            #      CMAKE_CFG_INTDIR and run ctest through vcbuild:
            #      
            #      $ vcbuild RUN_TESTS.vcproj "Win32|<build_config>"
            #
            #      If this is neccessary at all. I still think the above
            #      command line does exactly the right thing without our
            #      interference.
            #      I'm on Mac OS right now so I can't test it and I don't want
            #      to break the build. Buddah is smiling so nicely. [DS]
            get_target_property(
                TEST_LOCATION ${TEST_NAME} LOCATION_${CMAKE_BUILD_TYPE}
            )
	    
            # tell ctest about it
            add_test(${THIS_LIBRARY_NAME}_${TEST} ${TEST_LOCATION})
        endforeach(TEST)
    endif(ACMAKE_BUILD_TESTS)

    # add our target to the current project
    if(NOT THIS_LIBRARY_DONT_INSTALL)
        ac_project_add_target(
            LIBRARIES ${THIS_LIBRARY_NAME}
            EXTERNS ${THIS_LIBRARY_EXTERNS}
            DEPENDS ${THIS_LIBRARY_DEPENDS}
        )
    endif(NOT THIS_LIBRARY_DONT_INSTALL)

endmacro(ac_add_library)


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
# Debian packaging
#
# 1. Implementation
#
# This subsystem supports the creation of policy-compliant
# debian and ubuntu packages from cmake install components.
#
# It uses the cdbs build system to create the packages,
# which keeps most complexity out of here.
#
# 2. Assumptions
#
# It is expected that all shared libraries provide SOVERSION
# to be compliant with debian shared library packaging rules.
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

# assume that every linux can build debian packages
if(LINUX)
    set(ACMAKE_DEBIAN_SUPPORTED YES)
endif(LINUX)

# provide an option if packaging is supported
if(ACMAKE_DEBIAN_SUPPORTED)
    option(ACMAKE_DEBIAN "Build debian packages" NO)
endif(ACMAKE_DEBIAN_SUPPORTED)

# initialize globals if packaging is enabled
if(ACMAKE_DEBIAN)
    # debugging support
    ac_define_debug_channel(debian "Debug debian packaging")
    
    # location of debian build infrastructure
    set(ACMAKE_DEBIAN_DIR       "${CMAKE_BINARY_DIR}/debian")
    set(ACMAKE_DEBIAN_CONTROL   "${ACMAKE_DEBIAN_DIR}/control")
    set(ACMAKE_DEBIAN_RULES     "${ACMAKE_DEBIAN_DIR}/rules")
    set(ACMAKE_DEBIAN_CHANGELOG "${ACMAKE_DEBIAN_DIR}/changelog")

    set(ACMAKE_SOURCE_DEBIAN_DIR       "${CMAKE_SOURCE_DIR}/debian")
    set(ACMAKE_SOURCE_DEBIAN_CHANGELOG "${ACMAKE_SOURCE_DEBIAN_DIR}/changelog")

    # create infrastructure directory
    make_directory(${ACMAKE_DEBIAN_DIR})

    # ensure that there is a changelog
    if(NOT EXISTS ${ACMAKE_SOURCE_DEBIAN_CHANGELOG})
        message(FATAL_ERROR "You did not provide a debian changelog, which is required.")
    endif(NOT EXISTS ${ACMAKE_SOURCE_DEBIAN_CHANGELOG})

    if(NOT ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
        copy_file(${ACMAKE_SOURCE_DEBIAN_CHANGELOG} ${ACMAKE_DEBIAN_CHANGELOG})
    endif(NOT ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

else(ACMAKE_DEBIAN)
    # do not define anything unless enabled
    return()
endif(ACMAKE_DEBIAN)


# Define the one and only debian source package
#
# ac_debian_define_source(
#     name
#     SECTION section
#     PRIORITY priority
#     MAINTAINER maintainer
#     HOMEPAGE homepage
#     UPLOADERS uploader ...
#     [ BUILD_DEPENDS build_dependency ... ]
# )
#
# Properties are dumped directly into the control file.
# Their definition is beyond the scope of this documentation.
# Correctness will not be verified except for induced failure.
#
function(ac_debian_define_source NAME)
    # ignore if not building for debian
    if(NOT ACMAKE_DEBIAN)
        return()
    endif(NOT ACMAKE_DEBIAN)

    ac_debug_debian("Defining source package ${NAME}")

    # ensure there will only ever be one source package
    get_global(ACMAKE_DEBIAN_SOURCE_PACKAGE S)
    if(S)
        message(FATAL_ERROR "Cannot add debian source package ${NAME} because there already is one called ${S}")
    endif(S)
    set_global(ACMAKE_DEBIAN_SOURCE_PACKAGE ${NAME})

    # parse arguments
    parse_arguments(
        THIS
        "SECTION;PRIORITY;MAINTAINER;HOMEPAGE;UPLOADERS;BUILD_DEPENDS"
        "PROVIDE_DEBUG_PACKAGE"
        ${ARGN}
    )

    # our packages always depend on cmake and cdbs
    list(APPEND THIS_BUILD_DEPENDS cdbs cmake)

    # write out the source part of the control file
    set(C "${ACMAKE_DEBIAN_CONTROL}")

    file(WRITE ${C} "")

    _ac_deb_props(${C}
        "Source"     ${NAME}
        "Section"    ${THIS_SECTION}
        "Priority"   ${THIS_PRIORITY}
        "Maintainer" ${THIS_MAINTAINER}
        "Homepage"   ${THIS_HOMEPAGE}
    )

    _ac_deb_list(${C} "Uploaders"     ${THIS_UPLOADERS})
    _ac_deb_list(${C} "Build-Depends" ${THIS_BUILD_DEPENDS})

    _ac_deb_break(${C})

    if(THIS_PROVIDE_DEBUG_PACKAGE)
        ac_debug_debian("Defining debug package ${NAME}-dbg")
        set_global(ACMAKE_DEBIAN_DEBUG_PACKAGE ${NAME}-dbg)
        ac_debian_add_package(
            ${NAME}-dbg
        )
    endif(THIS_PROVIDE_DEBUG_PACKAGE)
endfunction(ac_debian_define_source)

# Define a debian binary package
#
# ac_debian_add_package(
#     name
#     ARCHITECTURE architecture
#     DESCRIPTION description
#     COMPONENTS component ...
#     [ DEPENDS dependency ... ]
#     [ RECOMMENDS recommended ... ]
#     [ PROVIDES provided ... ]
#     [ BLURB blurbline ... ]
# )
#
# Properties are dumped directly into the control file.
# Their definition is beyond the scope of this documentation.
# Correctness will not be verified except for induced failure.
#
# COMPONENTS lists cmake install components to be installed for the package
#
function(ac_debian_add_package NAME)
    # ignore if not building for debian
    if(NOT ACMAKE_DEBIAN)
        return()
    endif(NOT ACMAKE_DEBIAN)

    ac_debug_debian("Defining binary package ${NAME}")

    # there must be a source package
    get_global(ACMAKE_DEBIAN_SOURCE_PACKAGE S)
    if(NOT S)
        message(FATAL_ERROR "A debian source package must be added before adding debian package ${NAME}")
    endif(NOT S)

    # parse arguments
    parse_arguments(
        THIS
        "ARCHITECTURE;DESCRIPTION;DEPENDS;RECOMMENDS;PROVIDES;LONG_DESCRIPTION;COMPONENTS"
        "SHARED_LIBRARY_DEPENDENCIES;MISC_DEPENDENCIES"
        ${ARGN}
    )

    # add ourselves to global list of packages
    append_global(ACMAKE_DEBIAN_PACKAGES ${NAME})

    # default architecture to all
    if(NOT THIS_ARCHITECTURE)
        set(THIS_ARCHITECTURE any)
    endif(NOT THIS_ARCHITECTURE)

    # default description and warning if missing
    if(NOT THIS_DESCRIPTION)
        message("Debian package ${NAME} does not provide a description.")
        set(THIS_DESCRIPTION "No description.")
    endif(NOT THIS_DESCRIPTION)

    # default blurb and warning if missing
    if(NOT THIS_LONG_DESCRIPTION)
        message("Debian package ${NAME} does not provide a long description.")
        set(THIS_LONG_DESCRIPTION "No description.")
    endif(NOT THIS_LONG_DESCRIPTION)

    # implement shlibdeps
    if(THIS_SHARED_LIBRARY_DEPENDENCIES)
        list(APPEND THIS_DEPENDS "\${shlibs:Depends}")
    endif(THIS_SHARED_LIBRARY_DEPENDENCIES)

    # implement miscdeps
    if(THIS_MISC_DEPENDENCIES)
        list(APPEND THIS_DEPENDS "\${misc:Depends}")
    endif(THIS_MISC_DEPENDENCIES)

    # remember which components to install
    if(THIS_COMPONENTS)
        set_global(ACMAKE_DEBIAN_PACKAGE_${NAME}_COMPONENTS ${THIS_COMPONENTS})
    endif(THIS_COMPONENTS)

    # write our control file section
    set(C "${ACMAKE_DEBIAN_CONTROL}")

    _ac_deb_props(${C}
        "Package" ${NAME}
        "Architecture" ${THIS_ARCHITECTURE}
        "Description" ${THIS_DESCRIPTION}
    )

    _ac_deb_list(${C} "Depends" ${THIS_DEPENDS})
    _ac_deb_list(${C} "Recommends" ${THIS_RECOMMENDS})
    _ac_deb_list(${C} "Provides" ${THIS_PROVIDES})

    _ac_deb_text(${C} ${THIS_LONG_DESCRIPTION})

    _ac_deb_break(${C})

endfunction(ac_debian_add_package)

# Finalize debian packaging infrastructure
#
# ac_debian_finalize()
#
# This may modify both the debian directory and cmake state.
#
function(ac_debian_finalize)
    # ignore if not building debian packages
    if(NOT ACMAKE_DEBIAN)
        return()
    endif(NOT ACMAKE_DEBIAN)

    ac_debug_debian("Finalizing debian packaging")

    get_global(ACMAKE_DEBIAN_PACKAGES PACKAGES)

    # for each package, generate one command per component
    # that installs it into its debian package directory
    set(INSTALL_COMMANDS)
    foreach(P ${PACKAGES})
        ac_debug_debian("  Finalizing installation of package ${P}")
        get_global(ACMAKE_DEBIAN_PACKAGE_${P}_COMPONENTS COMPONENTS)
        foreach(C ${COMPONENTS})
            ac_debug_debian("    Package ${P} will contain component ${C}")
            set(COMMAND
                cmake "-DCMAKE_INSTALL_PREFIX=${ACMAKE_DEBIAN_DIR}/${P}" "-DCMAKE_INSTALL_COMPONENT=${C}" -P cmake_install.cmake
            )
            list(APPEND INSTALL_COMMANDS COMMAND ${COMMAND})
        endforeach(C ${COMPONENTS})
    endforeach(P ${ACMAKE_DEBIAN_PACKAGES})

    # register install commands in a target, which will be
    # built by debian/rules to install all packages
    add_custom_target(
        install-debian
        ${INSTALL_COMMANDS}
        COMMENT "Installing to debian package directories"
        VERBATIM
    )

    # generate the rules file
    _ac_debian_generate_rules()
endfunction(ac_debian_finalize)

### RULES FILE GENERATION ###

# Generate the debian rules file
function(_ac_debian_generate_rules)
    ac_debug_debian("Generating rules file")

    get_global(ACMAKE_DEBIAN_DEBUG_PACKAGE DEBUG)

    if(DEBUG)
        set(BUILD_TYPE "RelWithDebInfo")
    else(DEBUG)
        set(BUILD_TYPE "Release")
    endif(DEBUG)

    file(WRITE ${ACMAKE_DEBIAN_RULES}
"#!/usr/bin/make -f

include /usr/share/cdbs/1/rules/debhelper.mk
include /usr/share/cdbs/1/class/cmake.mk

# use our custom target for installation
DEB_MAKE_INSTALL_TARGET=install-debian

# always build in the generated tree
DEB_SRCDIR=.
DEB_BUILDDIR=.

# XXX
#  override standard arguments because they override compiler choice, which leads
#  to devastating reconfigures that somehow drop our later configuration on the way
DEB_CMAKE_NORMAL_ARGS=-DCMAKE_INSTALL_PREFIX=$(DEB_CMAKE_INSTALL_PREFIX) -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_VERBOSE_MAKEFILES=YES

# configure acmake for debian packaging
DEB_CMAKE_EXTRA_FLAGS=-DACMAKE_DEBIAN_PACKAGES=YES -DACMAKE_BUILD_TESTS=NO -DY60_WITH_GTK=NO -DACMAKE_DEBUG_DEBIAN=YES

"
    )

    if(DEBUG)
        file(APPEND ${ACMAKE_DEBIAN_RULES} "DEB_DH_STRIP_ARGS=--dbg-package=${DEBUG}\n")
    endif(DEBUG)

    # XXX this is somewhat disgusting, although it should
    #     not pose a problem as we only run this on linux
    execute_process(COMMAND chmod +x "${ACMAKE_DEBIAN_RULES}")

endfunction(_ac_debian_generate_rules)

### CONTROL FILE GENERATION ###

# XXX: clean up names

function(_ac_deb_props FILE)
    list(LENGTH ARGN ARGC)
    
    math(EXPR LAST "${ARGC} - 1")

    set(I 0)
    while(I LESS ${LAST})
        math(EXPR J "${I} + 1")
        list(GET ARGN ${I} PROPERTY)
        list(GET ARGN ${J} VALUE)
        file(APPEND ${FILE} "${PROPERTY}: ${VALUE}\n")
        math(EXPR I "${I} + 2")
    endwhile(I LESS ${LAST})
endfunction(_ac_deb_props)

function(_ac_deb_list FILE NAME)
    list(LENGTH ARGN ARGC)

    if(ARGC EQUAL 0)
        return()
    endif(ARGC EQUAL 0)

    file(APPEND ${FILE} "${NAME}: ")

    math(EXPR LAST "${ARGC} - 1")

    set(I 0)
    while(I LESS ${LAST})
        list(GET ARGN ${I} VALUE)
        file(APPEND ${FILE} "${VALUE}, ")
        math(EXPR I "${I} + 1")
    endwhile(I LESS ${LAST})

    list(GET ARGN ${LAST} VALUE)
    file(APPEND ${FILE} " ${VALUE}\n")
endfunction(_ac_deb_list)

function(_ac_deb_text FILE)
    foreach(LINE ${ARGN})
        file(APPEND ${FILE} " ${LINE}\n")
    endforeach(LINE)
endfunction(_ac_deb_text)

function(_ac_deb_break FILE)
    file(APPEND ${FILE} "\n")
endfunction(_ac_deb_break)

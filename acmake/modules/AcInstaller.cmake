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
# Subsystem for CPack integration.
#
# This is comprised of two things:
#  - solution declarators (used to declare a toplevel point
#    of reference for packaging multiple projects into a common
#    installer)
#  - installer declarators (used internally at all points that
#    might yield a package: solutions and projects)
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

# Debug channel
ac_define_debug_channel(installer "Installer framework debugging")

# Option for disabling the whole installer subsystem.
option(ACMAKE_BUILD_PACKAGES "Allow building packages with CPack" YES)

# Declare a solution, initializing AcInstaller to run in solution mode.
# Note that there can only be one solution per build environment.
#
# ac_begin_solution(name)
#
# The NAME of the solution is used to name the installer.
#
macro(ac_begin_solution NAME)
    # there can only be one solution
    get_global(ACMAKE_SOLUTION SOLUTION)
    if(SOLUTION)
        message(FATAL_ERROR "Trying to define more than one solution in a build")
    endif(SOLUTION)

    # declare the solution in global state
    set_global(ACMAKE_SOLUTION ${NAME})

    # set up CPack, passing along our rest argument
    _ac_declare_installer(${NAME} ${ARGN})
endmacro(ac_begin_solution)

# Finishes a solution.
#
# ac_end_solution(name)
#
# The NAME must match an earlier solution declaration.
#
macro(ac_end_solution NAME)
    # check for proper blocking
    get_global(ACMAKE_SOLUTION SOLUTION)
    if(NOT SOLUTION STREQUAL ${NAME})
        message(FATAL_ERROR "Trying to end the solution ${NAME} without beginning it")
    endif(NOT SOLUTION STREQUAL ${NAME})
    # check for unfinished projects
    if(ACMAKE_CURRENT_PROJECT)
        message(FATAL_ERROR "Finishing solution ${NAME} with unfinished project ${ACMAKE_CURRENT_PROJECT}")
    endif(ACMAKE_CURRENT_PROJECT)
endmacro(ac_end_solution)

# Declare an additional install type.
#
# ac_add_install_type(
#  name
#  DISPLAY_NAME displayname
# )
#
# Declares an additional type of installation.
# These can be mentioned in components to
# provide a pre-selected set of them for
# specific installation use cases.
#
# This does not require a project context.
#
macro(ac_add_install_type NAME)
    if(ACMAKE_BUILD_PACKAGES)
        ac_debug_installer("Adding install type ${NAME}")

        cpack_add_install_type(${NAME} ${ARGN})
    endif(ACMAKE_BUILD_PACKAGES)
endmacro(ac_add_install_type)

# Declare an installer component in the current project.
#
# ac_add_installer_component(
#   name
#   DISPLAY_NAME displayname
#   DESCRIPTION  description
#   [ INSTALL_TYPES installtype ... ]
#   [ DEPENDS component ... ]
# )
#
# Called from project context, declares an additional
# component that can be used to group installed files.
#   
macro(ac_add_installer_component NAME)
    if(ACMAKE_BUILD_PACKAGES)
        parse_arguments(
            THIS_COMPONENT
            "DISPLAY_NAME;DESCRIPTION;DEPENDS;INSTALL_TYPES"
            ""
            ${ARGN}
        )
    
        ac_debug_installer("Adding installer component ${NAME} depending on ${THIS_COMPONENT_DEPENDS}")

        set(PROJECT ${ACMAKE_CURRENT_PROJECT})
        
        get_global(ACMAKE_SOLUTION SOLUTION)
        
        if(SOLUTION)
            set(GROUP_MEMBERSHIP GROUP "${PROJECT}")
        else(SOLUTION)
            set(GROUP_MEMBERSHIP)
        endif(SOLUTION)
        
        if(NOT THIS_COMPONENT_DISPLAY_NAME)
            message(FATAL_ERROR "Installer component ${NAME} from project ${PROJECT} has no display name.")
        endif(NOT THIS_COMPONENT_DISPLAY_NAME)
        
        if(NOT THIS_COMPONENT_DESCRIPTION)
            message(FATAL_ERROR "Installer component ${NAME} from project ${PROJECT} has no description.")
        endif(NOT THIS_COMPONENT_DESCRIPTION)
        
        if(NOT THIS_COMPONENT_INSTALL_TYPES)
            set(THIS_COMPONENT_INSTALL_TYPES runtime development)
        endif(NOT THIS_COMPONENT_INSTALL_TYPES)
        
        cpack_add_component(
            ${NAME}
            DISPLAY_NAME "${THIS_COMPONENT_DISPLAY_NAME}"
            DESCRIPTION  "${THIS_COMPONENT_DESCRIPTION}"
            INSTALL_TYPES ${THIS_COMPONENT_INSTALL_TYPES}
            DEPENDS       ${THIS_COMPONENT_DEPENDS}
            ${GROUP_MEMBERSHIP}
        )
    endif(ACMAKE_BUILD_PACKAGES)
endmacro(ac_add_installer_component)

if(WIN32)
    set(ACMAKE_SHORTCUT_FILE "${CMAKE_BINARY_DIR}/${ACMAKE_BINARY_SUBDIR}/Shortcuts.js")
endif(WIN32)

macro(_ac_init_installer_shortcuts)
    if(WIN32)
        file(WRITE ${ACMAKE_SHORTCUT_FILE} "// This generated file lists all shortcuts\n")
        install(
            FILES ${ACMAKE_SHORTCUT_FILE}
            DESTINATION lib/acmake/tools
            )
    endif(WIN32)
endmacro(_ac_init_installer_shortcuts)

macro(ac_add_installer_shortcut NAME ICON DESCRIPTION WORKINGDIR COMMAND ARGUMENTS)
    if(WIN32)
        file(APPEND ${ACMAKE_SHORTCUT_FILE} "defineShortcut(\"${NAME}\", \"${ICON}\", \"${DESCRIPTION}\", \"${WORKINGDIR}\", \"${COMMAND}\", \"${ARGUMENTS}\")\n")
    endif(WIN32)
endmacro(ac_add_installer_shortcut)

# Internal: handle installation for a specific project.
#
# This should be called by projects at some point
# to set up installer-specific things, including:
#
#  - project-specific install components and component groups
#  - cpack bootstrap (if not in a solution)
#
macro(_ac_package_project NAME)
    if(ACMAKE_BUILD_PACKAGES)
        get_global(ACMAKE_SOLUTION SOLUTION)

        # if we are not in a solution then we try to assume
        # responsibility for creating an installer based
        # on our project declaration
        if(NOT SOLUTION)
            # ensure we are not declaring multiple project packages outside a solution build
            get_global(ACMAKE_INSTALLER_PROJECT INSTALLER_PROJECT)
            if(INSTALLER_PROJECT)
                message(FATAL_ERROR "Trying to define more than one project for installation without a solution.")
            endif(INSTALLER_PROJECT)
            set_global(ACMAKE_INSTALLER_PROJECT ${NAME})
            
            # set up CPack, passing through the installer-specific
            # keyword arguments given to us by the project framework
            _ac_declare_installer(${NAME} ${ARGN})
        endif(NOT SOLUTION)

        # if we are in a solution then we put our
        # components into a project-specific group
        if(SOLUTION)
            cpack_add_component_group(
                "${NAME}"
                DISPLAY_NAME "${NAME}"
                DESCRIPTION "NO DESCRIPTION" # XXX: better argument handling can fix this
            )
            set(GROUP_MEMBERSHIP GROUP "${NAME}")
        endif(SOLUTION)
        
        # collect dependencies to other projects in this build
        set(DEVELOPMENT_DEPENDS)
        set(RUNTIME_DEPENDS)
        get_global(${NAME}_REQUIRED_PACKAGES REQ_PACKAGES)
        get_global(${NAME}_OPTIONAL_PACKAGES OPT_PACKAGES)
        foreach(PACKAGE ${REQ_PACKAGES} ${OPT_PACKAGES})
            get_global(${PACKAGE}_IS_PROJECT    IS_PROJECT)
            get_global(${PACKAGE}_IS_INTEGRATED IS_INTEGRATED)
            if(IS_PROJECT AND IS_INTEGRATED)
                list(APPEND DEVELOPMENT_DEPENDS "${PACKAGE}_development")
                list(APPEND RUNTIME_DEPENDS     "${PACKAGE}_runtime")
            endif(IS_PROJECT AND IS_INTEGRATED)
        endforeach(PACKAGE ${REQ_PACKAGES} ${OPT_PACKAGES})

        ac_is_integrated(AcMake ACMAKE_INTEGRATED)
        if(ACMAKE_INTEGRATED)
            set(ACMAKE_DEPENDENCY "AcMake")
        else(ACMAKE_INTEGRATED)
            set(ACMAKE_DEPENDENCY)
        endif(ACMAKE_INTEGRATED)

        # declare standard components that every
        # projects gets: one for the runtime and
        # one for development files
        cpack_add_component(
            "${NAME}_runtime"
            DISPLAY_NAME "${NAME} Runtime"
            DESCRIPTION  "Runtime files for ${NAME}"
            DEPENDS ${RUNTIME_DEPENDS}
            INSTALL_TYPES runtime development
            ${GROUP_MEMBERSHIP}
        )
        cpack_add_component(
            "${NAME}_development"
            DISPLAY_NAME "${NAME} Development"
            DESCRIPTION  "Development files for ${NAME}"
            DEPENDS "${NAME}_runtime" ${ACMAKE_DEPENDENCY} ${DEVELOPMENT_DEPENDS}
            INSTALL_TYPES development
            ${GROUP_MEMBERSHIP}
        )

    endif(ACMAKE_BUILD_PACKAGES)
endmacro(_ac_package_project)

# Internal: set up CPack to build installers
#
# This function is responsible for setting up global
# CPack variables and loading the CPack helper module.
#
# It also has the side effect of declaring the
# default install types "runtime" and "development".
#
macro(_ac_declare_installer NAME)
    # parse arguments
    parse_arguments(
        INSTALLER
        "CONTACT;VENDOR;VERSION_MAJOR;VERSION_MINOR;VERSION_PATCH;DESCRIPTION;DESCRIPTION_FILE;LICENSE_FILE;PACKAGE_ICON;INSTALLER_ICON;UNINSTALLER_ICON"
        ""
        ${ARGN}
    )

    ac_debug_installer("Setting up installer named ${NAME}")

    # check arguments
    if(NOT INSTALLER_CONTACT)
        message(FATAL_ERROR "You need to provide a contact mail address for the installer ${NAME}.")
    endif(NOT INSTALLER_CONTACT)
    if(NOT INSTALLER_VENDOR)
        message(FATAL_ERROR "You need to provide a package vendor for the installer ${NAME}.")
    endif(NOT INSTALLER_VENDOR)
    if(NOT INSTALLER_DESCRIPTION)
        message(FATAL_ERROR "You need to provide a package description for the installer ${NAME}.")
    endif(NOT INSTALLER_DESCRIPTION)

    if(ACMAKE_BUILD_PACKAGES)
        # set up cpack globals
        set(CPACK_PACKAGE_NAME              ${NAME})
        set(CPACK_PACKAGE_INSTALL_DIRECTORY ${NAME})

        set(CPACK_PACKAGE_CONTACT ${INSTALLER_CONTACT})
        set(CPACK_PACKAGE_VENDOR  ${INSTALLER_VENDOR})

        set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${INSTALLER_DESCRIPTION})
        set(CPACK_PACKAGE_DESCRIPTION_FILE    ${INSTALLER_DESCRIPTION_FILE})

        set(CPACK_RESOURCE_FILE_LICENSE       ${INSTALLER_LICENSE_FILE})

        set(CPACK_PACKAGE_VERSION_MAJOR "${INSTALLER_VERSION_MAJOR}")
        set(CPACK_PACKAGE_VERSION_MINOR "${INSTALLER_VERSION_MINOR}")
        set(CPACK_PACKAGE_VERSION_PATCH "${INSTALLER_VERSION_PATCH}")

        if(WIN32)
            set(CPACK_GENERATOR NSIS ZIP) 
        endif(WIN32)

        if(LINUX)
            set(CPACK_GENERATOR DEB RPM TGZ TBZ2)
        endif(LINUX)

        if(WIN32)
            _ac_init_installer_shortcuts()
        
            # XXX: configurable?
            set(CPACK_NSIS_MODIFY_PATH ON)

            file(TO_NATIVE_PATH "${INSTALLER_PACKAGE_ICON}" PACKAGE_ICON_NSIS)
            set(CPACK_PACKAGE_ICON     ${PACKAGE_ICON_NSIS})
            file(TO_NATIVE_PATH "${INSTALLER_INSTALLER_ICON}" INSTALLER_ICON_NSIS)
            set(CPACK_NSIS_MUI_ICON    ${INSTALLER_ICON_NSIS})
            file(TO_NATIVE_PATH "${INSTALLER_UNINSTALLER_ICON}" UNINSTALLER_ICON_NSIS)
            set(CPACK_NSIS_MUI_UNIICON ${UNINSTALLER_ICON_NSIS})

            set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
                ExecShell \\\"\\\" \\\"$INSTDIR\\\\lib\\\\acmake\\\\tools\\\\CreateShortcuts.wsf\\\" \\\"% \\\$INSTDIR % \\\$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\"
            ")
#             set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
#                 ExecShell \\\"\\\" \\\"$INSTDIR\\\\lib\\\\acmake\\\\tools\\\\RemoveShortcuts.wsf\\\" \\\"% \\\$INSTDIR % \\\$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\"
#             ")

        endif(WIN32)

        ac_debug_installer("CPack is being initialized")

        # load cpack helper
        include(CPack)

        # declare standard install types
        cpack_add_install_type(
            development
            DISPLAY_NAME "Development"
        )
        
        cpack_add_install_type(
            runtime
            DISPLAY_NAME "Runtime"
        )

    endif(ACMAKE_BUILD_PACKAGES)
endmacro(_ac_declare_installer)

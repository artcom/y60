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
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

if(NOT ACMAKE_DEFAULT_BUILD_CONFIG)
    set(ACMAKE_DEFAULT_BUILD_CONFIG Release)
endif(NOT ACMAKE_DEFAULT_BUILD_CONFIG)


# XXX uses undocumented behaviour: assumes CMAKE_CONFIGURATION_TYPES is 
#     empty for single configuration generators
if(CMAKE_CONFIGURATION_TYPES)
    # multi config generator
    set_global(ACMAKE_BUILD_CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES})
    option(ACMAKE_ENFORCE_DEFAULT_BUILD_CONFIG
            "Enforce the AcMake default configuration even for multi configuration generators"
            OFF)
else(CMAKE_CONFIGURATION_TYPES)
    # single config generator
    set_global(ACMAKE_BUILD_CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel)
endif(CMAKE_CONFIGURATION_TYPES)

# this is currently only useful for Xcode, where gcc meets a multi
# configuration generator
function(ac_register_build_configuration NAME)
    get_global(ACMAKE_BUILD_CONFIGURATIONS BUILD_CONFIGS)
    list(FIND BUILD_CONFIGS ${NAME} INDEX)
    if(NOT INDEX EQUAL -1)
        message(SEND_ERROR "build configuration '${NAME}' already exists.")
    else(NOT INDEX EQUAL -1)
        list(APPEND BUILD_CONFIGS ${NAME})
        set_global(ACMAKE_BUILD_CONFIGURATIONS ${BUILD_CONFIGS})
        if(CMAKE_CONFIGURATION_TYPES)
            list(APPEND CMAKE_CONFIGURATION_TYPES ${NAME})
        endif( CMAKE_CONFIGURATION_TYPES )
    endif(NOT INDEX EQUAL -1)
endfunction( ac_register_build_configuration)

function( _ac_guess_build_config_from_binary_dir OUT )
    get_global(ACMAKE_BUILD_CONFIGURATIONS BUILD_CONFIGS)
    set(GUESSED_CONFIG)
    string(TOUPPER "${CMAKE_BINARY_DIR}" BIN_DIR_UC)
    foreach(BCONFIG ${BUILD_CONFIGS})
        string(TOUPPER ${BCONFIG} BCONFIG_UC)
        if(BIN_DIR_UC MATCHES ".*/[^/]*[._]?${BCONFIG_UC}[/]?\$")
            message("-- ACMake: Guessing CMAKE_BUILD_CONFIG from directory name: ${BCONFIG}")
            set(GUESSED_CONFIG ${BCONFIG})
            break()
        endif(BIN_DIR_UC MATCHES ".*/[^/]*[._]?${BCONFIG_UC}[/]?\$")
    endforeach(BCONFIG ${BUILD_CONFIGS})
    set(${OUT} ${GUESSED_CONFIG} PARENT_SCOPE)
endfunction( _ac_guess_build_config_from_binary_dir OUT )

function(ac_done_registering_build_types)
    set(FINAL_CONFIG)
    if(CMAKE_BUILD_TYPE)
        set(FINAL_CONFIG ${CMAKE_BUILD_TYPE})
    else(CMAKE_BUILD_TYPE)
        if(CMAKE_CONFIGURATION_TYPES)
            # multi configuration genrators
            if(ACMAKE_ENFORCE_DEFAULT_BUILD_CONFIG)
                set(FINAL_CONFIG ${ACMAKE_DEFAULT_BUILD_CONFIG})
            endif(ACMAKE_ENFORCE_DEFAULT_BUILD_CONFIG)
        else(CMAKE_CONFIGURATION_TYPES)
            # single configuration genrators
            _ac_guess_build_config_from_binary_dir(FINAL_CONFIG)
            if(NOT FINAL_CONFIG)
                set(FINAL_CONFIG ${ACMAKE_DEFAULT_BUILD_CONFIG})
            endif(NOT FINAL_CONFIG)
        endif(CMAKE_CONFIGURATION_TYPES)
    endif(CMAKE_BUILD_TYPE)

    # compose a new message containing acmake- and user-defined build
    # configs
    set(BUILD_CONFIG_MSG "Choose the type of build, options are:")
    get_global(ACMAKE_BUILD_CONFIGURATIONS BUILD_CONFIGS)
    foreach(BCONFIG ${BUILD_CONFIGS})
        set(BUILD_CONFIG_MSG "${BUILD_CONFIG_MSG} ${BCONFIG}")
    endforeach(BCONFIG ${BUILD_CONFIGS})
    set(BUILD_CONFIG_MSG "${BUILD_CONFIG_MSG}.")

    # enforce a default. Ok, according to CMake FAQ
    set(CMAKE_BUILD_TYPE ${FINAL_CONFIG} CACHE STRING "${BUILD_CONFIG_MSG}" FORCE)

    ac_configure_file(
            "${ACMAKE_TEMPLATES_DIR}/AcBuildConfiguration.h.in"
            "${CMAKE_BINARY_DIR}/include/acmake/build_configuration.h"
            "ac_create_build_config_header()"
    )
endfunction(ac_done_registering_build_types)


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
# Toplevel module for including all of acmake.
#
# Also loads ctest and pkg-config support (XXX: re-evaluate).
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

# include guard
get_property(ACMAKE_PRESENT GLOBAL PROPERTY ACMAKE_PRESENT)
set_property(GLOBAL PROPERTY ACMAKE_PRESENT TRUE)

mark_as_advanced(ACMAKE_LOCKED_AND_LOADED ACMAKE_PRESENT)

if(ACMAKE_LOCKED_AND_LOADED)
    return()
else(ACMAKE_LOCKED_AND_LOADED)
    set(ACMAKE_LOCKED_AND_LOADED TRUE)
    if(ACMAKE_PRESENT)
        message("This build contains at least two projects using acmake. You should include it at toplevel.")
    endif(ACMAKE_PRESENT)
endif(ACMAKE_LOCKED_AND_LOADED)

# load pkg-config
include(FindPkgConfig)

# Make sure we have our templates and tools available.
# For an integrated build, this just means setting some vars.
# For a separate build, we load the installed config file.
if(NOT ACMAKE_INTEGRATED_BUILD)
    include(AcMakeConfig) # XXX this should probably use the same config file mechanism like pro60_deps
else(NOT ACMAKE_INTEGRATED_BUILD)
    set(ACMAKE_TOOLS_DIR ${AcMake_SOURCE_DIR}/tools)
    set(ACMAKE_MODULES_DIR ${AcMake_SOURCE_DIR}/modules)
    set(ACMAKE_TEMPLATES_DIR ${AcMake_SOURCE_DIR}/templates)
    set(ACMAKE_INCLUDE_DIR ${AcMake_SOURCE_DIR}/include)
endif(NOT ACMAKE_INTEGRATED_BUILD)

# Store generated files in this subdirectory of ${CMAKE_CURRENT_BINARY_DIR}
set(ACMAKE_BINARY_SUBDIR "ACMakeFiles")

# All sources may include acmake headers
include_directories(${ACMAKE_INCLUDE_DIR} ${CMAKE_BINARY_DIR}/include/)

include(AcBoostUtils)
include(AcFileUtils)
include(AcVariableUtils)

include(AcPlatform)
include(AcCompiler)

include(AcTesting)
include(AcTarget)
include(AcProject)
include(AcBuildinfo)

include(AcAddExecutable)
include(AcAddLibrary)
include(AcAddPlugin)

include(AcCoverage)
include(AcProfiling)

include(AcDocumentation)

include(AcBundleResources) # XXX deprecate this. It's shit. I'know it because I wrote it [DS]

include(AcAddPro60Dependencies)

ac_create_build_config_header()

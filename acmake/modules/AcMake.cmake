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

# load testing stuff
include(CTest)
mark_as_advanced(DART_TESTING_TIMEOUT)

# load pkg-config
include(FindPkgConfig)

# Force default buildtype to Release.
# Change helpstring to show Profiling and Coverage
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Profiling [gcc] Coverage [gcc]." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# Make sure we have our templates and tools available.
# For an integrated build, this just means setting some vars.
# For a separate build, we load the installed config file.
if(NOT ACMAKE_INTEGRATED_BUILD)
    include(AcMakeConfig)
else(NOT ACMAKE_INTEGRATED_BUILD)
    set(ACMAKE_TOOLS_DIR ${AcMake_SOURCE_DIR}/tools)
    set(ACMAKE_MODULES_DIR ${AcMake_SOURCE_DIR}/modules)
    set(ACMAKE_TEMPLATES_DIR ${AcMake_SOURCE_DIR}/templates)
    set(ACMAKE_INCLUDE_DIR ${AcMake_SOURCE_DIR}/include)
endif(NOT ACMAKE_INTEGRATED_BUILD)

# Store generated files in this subdirectory of ${CMAKE_CURRENT_BINARY_DIR}
set(ACMAKE_BINARY_SUBDIR "ACMakeFiles")

# All sources may include acmake headers
include_directories(${ACMAKE_INCLUDE_DIR})

include(AcPlatform)
include(AcCompiler)
include(AcBoostUtils)
include(AcVariableUtils)

include(AcTarget)
include(AcProject)

include(AcAddExecutable)
include(AcAddLibrary)
include(AcAddPlugin)

include(AcCoverage)
include(AcProfiling)

include(AcDocumentation)

include(AcBuildinfo)
include(AcBundleResources)

include(AcAddPro60Dependencies)


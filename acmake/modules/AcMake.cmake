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

include(CTest)

include(FindPkgConfig)

# Make sure we have our templates and tools available.
# For an integrated build, this just means setting some vars.
# For a separate build, we load the installed config file.
if(NOT ACMAKE_INTEGRATED_BUILD)
    include(AcMakeConfig)
else(NOT ACMAKE_INTEGRATED_BUILD)
    set(ACMAKE_TOOLS_DIR ${AcMake_SOURCE_DIR}/tools)
    set(ACMAKE_MODULES_DIR ${AcMake_SOURCE_DIR}/modules)
    set(ACMAKE_TEMPLATES_DIR ${AcMake_SOURCE_DIR}/templates)
endif(NOT ACMAKE_INTEGRATED_BUILD)

include(AcPlatform)
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

include(AcAddWindowsDependencies)


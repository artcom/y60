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
# Toplevel module for including all of acmake.
#
# Also loads pkg-config support (XXX: re-evaluate).
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#


### INCLUDE GUARD

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


### LOAD STANDARD MODULES

include(FindPkgConfig)


### LOAD ACMAKE

include(AcGlobal)

include(AcDebug)

include(AcBoostUtils)
include(AcFileUtils)
include(AcVariableUtils)

include(AcPlatform)
include(AcCompiler)

include(AcTesting)
include(AcTarget)
include(AcProject)
include(AcBuildinfo)
include(AcBuildConfiguration)

include(AcAddExecutable)
include(AcAddLibrary)
include(AcAddPlugin)

include(AcCoverage)
include(AcProfiling)

ac_done_registering_build_types()

include(AcDocumentation)

include(AcInstaller)

include(AcAddPro60Dependencies)

include(AcDebian)

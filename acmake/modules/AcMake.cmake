#============================================================================
# Copyright (C) 2008, ART+COM Technologies GmbH Berlin
#
# These coded instructions, statements, and computer programs contain
# unpublished proprietary information of ART+COM AG Berlin, and
# are copy protected by law. They may not be disclosed to third parties
# or copied or duplicated in any form, in whole or in part, without the
# specific, prior written permission of ART+COM AG Berlin.
#============================================================================
#
# Toplevel module for including all of acmake.
#
# Also makes a few standard cmake modules available.
#
#============================================================================

enable_testing()

include(FindPkgConfig)

if(NOT ACMAKE_INTEGRATED_BUILD)
    include(AcMakeConfig)
else(NOT ACMAKE_INTEGRATED_BUILD)
    set(ACMAKE_TOOLS_DIR ${AcMake_SOURCE_DIR}/tools)
    set(ACMAKE_MODULES_DIR ${AcMake_SOURCE_DIR}/modules)
    set(ACMAKE_TEMPLATES_DIR ${AcMake_SOURCE_DIR}/templates)
endif(NOT ACMAKE_INTEGRATED_BUILD)

include(AcPlatform)
include(AcUtils)

include(AcCommon)
include(AcAddExecutable)
include(AcAddLibrary)
include(AcAddPlugin)
include(AcBuildTypes)
include(AcDocumentation)


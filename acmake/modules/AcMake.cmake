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

include(AcPlatform)
include(AcUtils)

include(AcCommon)
include(AcAddExecutable)
include(AcAddLibrary)
include(AcAddPlugin)

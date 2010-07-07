#============================================================================
# Copyright (C) 2008, ART+COM Technologies GmbH Berlin
#
# These coded instructions, statements, and computer programs contain
# unpublished proprietary information of ART+COM AG Berlin, and
# are copy protected by law. They may not be disclosed to third parties
# or copied or duplicated in any form, in whole or in part, without the
# specific, prior written permission of ART+COM AG Berlin.
#============================================================================

set(ACMAKE_BUILDINFO OFF)

add_subdirectory(paintlib)
add_subdirectory(SDL_ttf2)
add_subdirectory(spidermonkey)
add_subdirectory(oscpack)

if(WIN32)
    add_subdirectory(fakeatl)
endif(WIN32)

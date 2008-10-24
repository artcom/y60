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
# This file declares cmake- and c-level discrimination
# variables for each platform supported by acmake.
#
# It also cares about various C low-level spells required
# for what we consider a sane C runtime environment:
#
#  - libc should be reentrant / threadsafe
#  - libm should be available and linked
#  - pthreads should be available (XXX: need pthread-win32 on windows)
#  - posix realtime library should be available
#  - dlopen should be available (XXX: not on windows)
#
#============================================================================

if(UNIX)

  if(APPLE)

    set(OSX true)
    add_definitions(-DOSX)

  else(APPLE)

    set(LINUX true)
    add_definitions(-DLINUX)

    # we consider libm and some others standard
    link_libraries(m dl rt pthread)

    # always be reentrant
    add_definitions(-D_REENTRANT)

    # may the GNU be with us
    add_definitions(-D_GNU_SOURCE)

    # XXX: we should really really do this.
    #add_definitions(-D_FILE_OFFSET_BITS=64)
  endif(APPLE)

else(UNIX)

  if(WIN32)
  endif(WIN32)

endif(UNIX)

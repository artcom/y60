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
# Helper macros common to various acmake subsystems.
#
#============================================================================

macro(_ac_attach_depends TARGET)
  target_link_libraries(${TARGET} ${ARGN})
endmacro(_ac_attach_depends)

macro(_ac_attach_externs TARGET)
  foreach(EXTERN ${ARGN})
    link_directories(${${EXTERN}_LIBRARY_DIRS})
    include_directories(${${EXTERN}_INCLUDE_DIRS})
    target_link_libraries(${TARGET} ${${EXTERN}_LIBRARIES})
  endforeach(EXTERN)
endmacro(_ac_attach_externs)

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
# See AcAddExecutable.cmake as an example on how to use these.
#
#============================================================================

# attach local local libraries to TARGET
macro(_ac_attach_depends TARGET)
    foreach(DEPEND ${ARGN})
        get_target_property(DEPEND_TYPE ${DEPEND} TYPE)
        if("XXX${DEPEND_TYPE}XXX" STREQUAL "XXXSHARED_LIBRARYXXX"
                OR "XXX${DEPEND_TYPE}XXX" STREQUAL "XXXSTATIC_LIBRARYXXX")
            target_link_libraries(${TARGET} ${DEPEND})
        endif("XXX${DEPEND_TYPE}XXX" STREQUAL "XXXSHARED_LIBRARYXXX"
            OR "XXX${DEPEND_TYPE}XXX" STREQUAL "XXXSTATIC_LIBRARYXXX")
    endforeach(DEPEND)
endmacro(_ac_attach_depends)

# attach external libraries to TARGET
macro(_ac_attach_externs TARGET)
    foreach(EXTERN ${ARGN})
        if(EXTERN MATCHES ".*\\.framework/?$")
            target_link_libraries(${TARGET} ${EXTERN})
        else(EXTERN MATCHES ".*\\.framework/?$")
            target_link_libraries(${TARGET} ${${EXTERN}_LIBRARIES})
        endif(EXTERN MATCHES ".*\\.framework/?$")
    endforeach(EXTERN)
endmacro(_ac_attach_externs)

# declare searchpath for headers and libraries for the *upcoming* target
macro(_ac_declare_searchpath)
    foreach(EXTERN ${ARGN})
        if(NOT EXTERN MATCHES ".*\\.framework/?$")
            link_directories(${${EXTERN}_LIBRARY_DIRS})
            include_directories(${${EXTERN}_INCLUDE_DIRS})
        endif(NOT EXTERN MATCHES ".*\\.framework/?$")
    endforeach(EXTERN)
endmacro(_ac_declare_searchpath)

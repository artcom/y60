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
# Helper macros common to various acmake subsystems.
# See AcAddExecutable.cmake as an example on how to use these.
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

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

    #message("_ac_attach_externs for ${TARGET} libs: ${ARGN}")

    foreach(EXTERN ${ARGN})
        if(EXTERN MATCHES ".*\\.framework/?$")
            target_link_libraries(${TARGET} ${EXTERN})
        else(EXTERN MATCHES ".*\\.framework/?$")
            target_link_libraries(${TARGET} ${${EXTERN}_LIBRARIES})
        endif(EXTERN MATCHES ".*\\.framework/?$")
        #message("defs for ${TARGET}: from lib ${EXTERN} defs: ${${EXTERN}_DEFINITIONS}")
        if ( ${EXTERN}_DEFINITIONS )

            set_target_properties( ${TARGET} 
                    PROPERTIES
                        COMPILE_DEFINITIONS ${${EXTERN}_DEFINITIONS} )
        endif ( ${EXTERN}_DEFINITIONS )
    endforeach(EXTERN)
endmacro(_ac_attach_externs)

# declare searchpath for headers and libraries for the *upcoming* target
macro(_ac_declare_searchpath)
    foreach(EXTERN ${ARGN})
        if(NOT EXTERN MATCHES ".*\\.framework/?$")
            if(${EXTERN}_LIBRARY_DIRS)
                link_directories(${${EXTERN}_LIBRARY_DIRS})
            endif(${EXTERN}_LIBRARY_DIRS)
            if(${EXTERN}_INCLUDE_DIRS)
                include_directories(${${EXTERN}_INCLUDE_DIRS})
            endif(${EXTERN}_INCLUDE_DIRS)
            if(${EXTERN}_LIBRARY_DIR)
                link_directories(${${EXTERN}_LIBRARY_DIR})
            endif(${EXTERN}_LIBRARY_DIR)
            if(${EXTERN}_INCLUDE_DIR)
                include_directories(${${EXTERN}_INCLUDE_DIR})
            endif(${EXTERN}_INCLUDE_DIR)
        endif(NOT EXTERN MATCHES ".*\\.framework/?$")
    endforeach(EXTERN)
endmacro(_ac_declare_searchpath)

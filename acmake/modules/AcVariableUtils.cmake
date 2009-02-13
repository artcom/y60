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
# Utility macros for easing access to non-variable properties.
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

macro(set_global NAME)
    set_property(GLOBAL PROPERTY ${NAME} ${ARGN})
endmacro(set_global NAME)

macro(append_global NAME)
    set_property(GLOBAL APPEND PROPERTY ${NAME} ${ARGN})
endmacro(append_global NAME)

macro(get_global NAME INTO)
    set(${INTO})
    get_property(${INTO} GLOBAL PROPERTY ${NAME})
endmacro(get_global NAME INTO)

# XXX: get rid of this
macro(print_global NAME)
    get_global(${NAME} MY_VALUE)
endmacro(print_global NAME)

macro(get_globals GLOBAL_PREFIX LOCAL_PREFIX)
    foreach(SUFFIX ${ARGN})
        get_global(${GLOBAL_PREFIX}_${SUFFIX} ${LOCAL_PREFIX}_${SUFFIX})
    endforeach(SUFFIX)
endmacro(get_globals GLOBAL_PREFIX LOCAL_PREFIX SUFFIXES)

macro(set_globals LOCAL_PREFIX GLOBAL_PREFIX)
    foreach(SUFFIX ${ARGN})
        set_global(${GLOBAL_PREFIX}_${SUFFIX} ${${LOCAL_PREFIX}_${SUFFIX}})
    endforeach(SUFFIX)
endmacro(set_globals LOCAL_PREFIX GLOBAL_PREFIX SUFFIXES)

macro(clear_locals LOCAL_PREFIX)
    foreach(SUFFIX ${ARGN})
        set(${LOCAL_PREFIX}_${SUFFIX})
    endforeach(SUFFIX)
endmacro(clear_locals LOCAL_PREFIX SUFFIXES)


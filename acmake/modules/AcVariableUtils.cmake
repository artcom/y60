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
# Utility macros for easing access to various kinds of properties.
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

### SCOPED PROPERTIES

# Shortcut for clearing several local vars with a common prefix
macro(clear_locals LOCAL_PREFIX)
    foreach(SUFFIX ${ARGN})
        set(${LOCAL_PREFIX}_${SUFFIX})
    endforeach(SUFFIX)
endmacro(clear_locals LOCAL_PREFIX SUFFIXES)

### GLOBAL PROPERTIES

# Shortcut for clearing a global property
macro(clear_global NAME)
    set_property(GLOBAL PROPERTY ${NAME})
endmacro(clear_global NAME)

# Shortcut for setting a global property
macro(set_global NAME)
    set_property(GLOBAL PROPERTY ${NAME} ${ARGN})
endmacro(set_global NAME)

# Shortcut for appending to a list in a global property
macro(append_global NAME)
    set_property(GLOBAL APPEND PROPERTY ${NAME} ${ARGN})
endmacro(append_global NAME)

# Remove an item from a global property list
macro(remove_global NAME VALUE)
    get_global(${NAME} _V)
    list(REMOVE_ITEM _V ${VALUE})
    set_global(${NAME} ${_V})
    set(_V)
endmacro(remove_global NAME VALUE)

# Shortcut for uniquely appending to a list in a global property
macro(append_global_unique NAME)
    get_global(${NAME} _TMP)
    list(APPEND _TMP ${ARGN})
    list(REMOVE_DUPLICATES _TMP)
    set_global(${NAME} ${_TMP})
endmacro(append_global_unique NAME)

# Shortcut for prepending to a list in a global property
macro(prepend_global NAME)
    get_global(${NAME} _V)
    list(INSERT _V 0 ${ARGN})
    set_global(${NAME} ${_V})
endmacro(prepend_global NAME)

# Shortcut for retrieving a global property
macro(get_global NAME INTO)
    set(${INTO})
    get_property(${INTO} GLOBAL PROPERTY ${NAME})
endmacro(get_global NAME INTO)

# Shortcut for copying a common-prefixed set of
# properties from the global namespace to scoped vars
macro(get_globals GLOBAL_PREFIX LOCAL_PREFIX)
    foreach(SUFFIX ${ARGN})
        get_global(${GLOBAL_PREFIX}_${SUFFIX} ${LOCAL_PREFIX}_${SUFFIX})
    endforeach(SUFFIX)
endmacro(get_globals GLOBAL_PREFIX LOCAL_PREFIX SUFFIXES)

# Shortcut for copying a common-prefixed set of
# properties from the scoped namespaces to global props
macro(set_globals LOCAL_PREFIX GLOBAL_PREFIX)
    foreach(SUFFIX ${ARGN})
        set_global(${GLOBAL_PREFIX}_${SUFFIX} ${${LOCAL_PREFIX}_${SUFFIX}})
    endforeach(SUFFIX)
endmacro(set_globals LOCAL_PREFIX GLOBAL_PREFIX SUFFIXES)

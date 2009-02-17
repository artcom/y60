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
# Handle PRO60 dependencies.
#
# XXX:This is pro60-specific and should not be in ACMake.
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

set(PRO60_DEPS $ENV{PRO60_DEPS})

if(PRO60_DEPS)

    list(INSERT CMAKE_PREFIX_PATH 0 ${PRO60_DEPS})

else(PRO60_DEPS)
    if(NOT LINUX)
        message(FATAL_ERROR "PRO60_DEPS environment variable not set")
    endif(NOT LINUX)
endif(PRO60_DEPS)

macro(pro60_deps_install)
    if(PRO60_DEPS)
        get_global(PRO60_DEPENDENCIES_WILL_BE_INSTALLED _WILL_BE_INSTALLED)
        if(NOT _WILL_BE_INSTALLED)
            add_subdirectory(${PRO60_DEPS} pro60-deps)
            set_global(PRO60_DEPENDENCIES_WILL_BE_INSTALLED YES)
        endif(NOT _WILL_BE_INSTALLED)
    endif(PRO60_DEPS)
endmacro(pro60_deps_install)


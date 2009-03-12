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
# Import PRO60 dependency package.
#
# XXX: This is pro60-specific and should not be in ACMake.
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

find_package(PRO60_DEPS NO_MODULE)

if(PRO60_DEPS_FOUND)
    pro60_deps_register_searchpath()
    mark_as_advanced(PRO60_DEPS_DIR PRO60_DEPS_ROOT_DIR)
    message("PRO60_DEPS found in ${PRO60_DEPS_ROOT_DIR}")
else(PRO60_DEPS_FOUND)
    if(WIN32)
        message(FATAL_ERROR "PRO60_DEPS could not be found")
    else(WIN32)
        message("PRO60_DEPS package could not be found.")
        if(ENV{PRO60_DEPS})
            list(INSERT CMAKE_PREFIX_PATH 0 $ENV{PRO60_DEPS})
        endif(ENV{PRO60_DEPS})
    endif(WIN32)
endif(PRO60_DEPS_FOUND)

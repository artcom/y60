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

if(LINUX)
    set(PRO60_DEPS $ENV{PRO60_DEPS})
    if(PRO60_DEPS)
        list(INSERT CMAKE_PREFIX_PATH 0 ${PRO60_DEPS})
    else(PRO60_DEPS)
        message(STATUS "PRO60_DEPS environment variable not set, dependencies must be in standard locations")
    endif(PRO60_DEPS)
endif(LINUX)

find_package( PRO60_DEPS NO_MODULE)
if(PRO60_DEPS_FOUND)
    pro60_deps_register_searchpath()
    message("PRO60_DEPS dir: ${PRO60_DEPS_ROOT_DIR}")
else(PRO60_DEPS_FOUND)
    message("ART+COM companion libraries not found")
endif(PRO60_DEPS_FOUND)




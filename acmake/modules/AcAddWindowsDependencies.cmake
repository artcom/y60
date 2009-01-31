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

# Add win32 dependency folder
if(WIN32)
    set(PRO60_DEPS $ENV{PRO60_DEPS})
    if(PRO60_DEPS)
        list(INSERT CMAKE_PREFIX_PATH 0 ${PRO60_DEPS})
        add_subdirectory( ${PRO60_DEPS} ${CMAKE_CURRENT_BINARY_DIR}/pro60_deps )
    else(PRO60_DEPS)
        message(SEND_ERROR "PRO60_DEPS environment variable not set.")
    endif(PRO60_DEPS)
endif(WIN32)


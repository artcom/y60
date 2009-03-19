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
# Some debugging utilities.
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

macro(ac_define_debug_channel NAME DESCRIPTION)
    string(TOUPPER ${NAME} _UNAME)
    option(ACMAKE_DEBUG_${_UNAME} "${DESCRIPTION}" OFF)
    function(ac_debug_${NAME} MESSAGE)
        if(ACMAKE_DEBUG_${_UNAME})
            message("${NAME}: ${MESSAGE}")
        endif(ACMAKE_DEBUG_${_UNAME})
    endfunction(ac_debug_${NAME})
endmacro(ac_define_debug_channel)

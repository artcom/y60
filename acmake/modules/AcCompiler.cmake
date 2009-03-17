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
# Compiler specific flags and switches
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

if(MSVC)
    # Full warnings
    add_definitions( /W4 )

    set(ACMAKE_PARALLEL_CL "" CACHE STRING "Number of processes to use when compiling with cl")

    if(ACMAKE_PARALLEL_CL)
        add_definitions( /MP${ACMAKE_PARALLEL_CL} )
    endif(ACMAKE_PARALLEL_CL)

endif(MSVC)

if(CMAKE_COMPILER_IS_GNUC OR CMAKE_COMPILER_IS_GNUCXX)
    # Compile faster: use pipes instead of intermediate files
    add_definitions( -pipe )

    # Full warnings
    add_definitions( -Wall )
endif(CMAKE_COMPILER_IS_GNUC OR CMAKE_COMPILER_IS_GNUCXX)

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
# Compiler specific flags and switches
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

if(MSVC)
    # Full warnings
    add_definitions( /W4 )
endif(MSVC)

if(CMAKE_COMPILER_IS_GNUC OR CMAKE_COMPILER_IS_GNUCXX)
    # Compile faster: use pipes instead of intermediate files
    add_definitions( -pipe )

    # Full warnings
    add_definitions( -Wall )
endif(CMAKE_COMPILER_IS_GNUC OR CMAKE_COMPILER_IS_GNUCXX)

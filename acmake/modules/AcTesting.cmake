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
# Testing support
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

option(ACMAKE_USE_CTEST "Use full ctest with cdash support?" NO)

if(ACMAKE_USE_CTEST)
    include(CTest)
else(ACMAKE_USE_CTEST)
    enable_testing()
endif(ACMAKE_USE_CTEST)

# XXX: this is implemented in an ugly way. targets should
#      be tagged as being for testing and should then
#      avoid registering themselves. instead, we handle
#      this in our shortcuts.
option(ACMAKE_BUILD_TESTS "Should tests be compiled?" YES)

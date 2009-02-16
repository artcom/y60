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
# Coverage build type (only for gcc)
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

if (CMAKE_COMPILER_IS_GNUC OR CMAKE_COMPILER_IS_GNUCXX)
    set(AC_COVERAGE_FLAGS "-fprofile-arcs -ftest-coverage")

    set(CMAKE_C_FLAGS_COVERAGE
        "${CMAKE_C_FLAGS_DEBUG} ${AC_COVERAGE_FLAGS}"
        CACHE STRING "Flags used by the C compiler during Coverage builds" FORCE)
    set(CMAKE_CXX_FLAGS_COVERAGE
        "${CMAKE_CXX_FLAGS_DEBUG} ${AC_COVERAGE_FLAGS}"
        CACHE STRING "Flags used by the C++ compiler during Coverage builds" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS_COVERAGE
        "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${AC_COVERAGE_FLAGS}"
        CACHE STRING "Flags used by the linker during Coverage builds" FORCE)
    set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE
        "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} ${AC_COVERAGE_FLAGS}"
        CACHE STRING "Flags used by the linker during Coverage builds" FORCE)
    set(CMAKE_MODULE_LINKER_FLAGS_COVERAGE
        "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} ${AC_COVERAGE_FLAGS}"
        CACHE STRING "Flags used by the linker during Coverage builds" FORCE)

    mark_as_advanced(
        CMAKE_C_FLAGS_COVERAGE
        CMAKE_CXX_FLAGS_COVERAGE
        CMAKE_EXE_LINKER_FLAGS_COVERAGE
        CMAKE_SHARED_LINKER_FLAGS_COVERAGE
        CMAKE_MODULE_LINKER_FLAGS_COVERAGE
    )
endif (CMAKE_COMPILER_IS_GNUC OR CMAKE_COMPILER_IS_GNUCXX)


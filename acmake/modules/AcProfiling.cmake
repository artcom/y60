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
# Profiling build type (only for gcc)
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

if (CMAKE_COMPILER_IS_GNUC OR CMAKE_COMPILER_IS_GNUCXX)
    set(AC_PROFILING_FLAGS "-pg")

    set(CMAKE_C_FLAGS_PROFILING
        "${CMAKE_C_FLAGS_DEBUG} ${AC_PROFILING_FLAGS}"
        CACHE STRING "Flags used by the C compiler during Profiling builds" FORCE)
    set(CMAKE_CXX_FLAGS_PROFILING
        "${CMAKE_CXX_FLAGS_DEBUG} ${AC_PROFILING_FLAGS}"
        CACHE STRING "Flags used by the C++ compiler during Profiling builds" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS_PROFILING
        "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${AC_PROFILING_FLAGS}"
        CACHE STRING "Flags used by the linker during Profiling builds" FORCE)
    set(CMAKE_SHARED_LINKER_FLAGS_PROFILING
        "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} ${AC_PROFILING_FLAGS}"
        CACHE STRING "Flags used by the linker during Profiling builds" FORCE)
    set(CMAKE_MODULE_LINKER_FLAGS_PROFILING
        "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} ${AC_PROFILING_FLAGS}"
        CACHE STRING "Flags used by the linker during Profiling builds" FORCE)

    mark_as_advanced(
        CMAKE_C_FLAGS_PROFILING
        CMAKE_CXX_FLAGS_PROFILING
        CMAKE_EXE_LINKER_FLAGS_PROFILING
        CMAKE_SHARED_LINKER_FLAGS_PROFILING
        CMAKE_MODULE_LINKER_FLAGS_PROFILING
    )

    ac_register_build_configuration( Profiling )
endif (CMAKE_COMPILER_IS_GNUC OR CMAKE_COMPILER_IS_GNUCXX)


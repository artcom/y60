#============================================================================
# Copyright (C) 2008, ART+COM Technologies GmbH Berlin
#
# These coded instructions, statements, and computer programs contain
# unpublished proprietary information of ART+COM AG Berlin, and
# are copy protected by law. They may not be disclosed to third parties
# or copied or duplicated in any form, in whole or in part, without the
# specific, prior written permission of ART+COM AG Berlin.
#============================================================================
#
# Defining some helpful build types like profiling.
#
#============================================================================

if (CMAKE_COMPILER_IS_GNUCXX)

    # add a "Profiling" build type
    set(CMAKE_C_FLAGS_PROFILING "${CMAKE_C_FLAGS_DEBUG} -pg" CACHE STRING "Flags used by the C compiler during Profiling builds" FORCE)
    set(CMAKE_CXX_FLAGS_PROFILING "${CMAKE_CXX_FLAGS_DEBUG} -pg" CACHE STRING "Flags used by the C++ compiler during Profiling builds" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS_PROFILING "${CMAKE_EXE_LINKER_FLAGS_DEBUG}" CACHE STRING "Flags used by the linker during Profiling builds." FORCE)
    set(CMAKE_SHARED_LINKER_FLAGS_PROFILING "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}" CACHE STRING "Flags used by the linker during Profiling builds" FORCE)
    set(CMAKE_MODULE_LINKER_FLAGS_PROFILING "${CMAKE_MODULE_LINKER_FLAGS_DEBUG}" CACHE STRING "Flags used by the linker during Profiling builds." FORCE)
    mark_as_advanced(
        CMAKE_C_FLAGS_PROFILING
        CMAKE_CXX_FLAGS_PROFILING
        CMAKE_EXE_LINKER_FLAGS_PROFILING
        CMAKE_SHARED_LINKER_FLAGS_PROFILING
        CMAKE_MODULE_LINKER_FLAGS_PROFILING
    )
endif (CMAKE_COMPILER_IS_GNUCXX)

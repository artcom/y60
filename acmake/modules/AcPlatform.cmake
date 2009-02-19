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
# This file declares cmake- and c-level discrimination
# variables for each platform supported by acmake.
#
# It also cares about various C low-level spells required
# for what we consider a sane C runtime environment:
#
#  - libc should be reentrant / threadsafe
#  - libm should be available and linked
#  - pthreads should be available (XXX: need pthread-win32 on windows)
#  - posix realtime library should be available
#  - dlopen should be available (XXX: not on windows)
#
# XXX: This file might need some re-thinking. Works for now though.
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

if(UNIX)
    if(APPLE)
        
        set(OSX true)
        add_definitions(-DOSX )
        
        # XXX CMake uses .so suffix on Mac OS X. We want .dylib
        set( CMAKE_SHARED_MODULE_SUFFIX ".dylib" )

        if( ${CMAKE_SYSTEM_PROCESSOR} MATCHES "^i[36]86$" )
            add_definitions( -DOSX_X86 )
        else( ${CMAKE_SYSTEM_PROCESSOR} MATCHES "^i[36]86$" )
            add_definitions( -DOSX_PPC )
        endif( ${CMAKE_SYSTEM_PROCESSOR} MATCHES "^i[36]86$" )
        
    else(APPLE)
        
        set(LINUX true)
        add_definitions(-DLINUX)
        
        # we consider libm and some others standard
        link_libraries(m dl rt pthread)
        
        # always be reentrant
        add_definitions(-D_REENTRANT)
        
        # may the GNU be with us
        add_definitions(-D_GNU_SOURCE)
        
        # XXX: we should really really do this.
        #add_definitions(-D_FILE_OFFSET_BITS=64)
    endif(APPLE)
else(UNIX)
    
    if(WIN32)
        # suppress superflous, verbose warnings
        add_definitions( -D_CRT_SECURE_NO_WARNINGS )
        add_definitions( -D_CRT_NONSTDC_NO_WARNINGS )
        add_definitions( -D_SCL_SECURE_NO_WARNINGS )
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
        # XXX TODO per plugin stuff
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

        # Generate a file containing some information captured from vsvars32.bat
        # XXX: move this somewhere else, preferably a "FindWindowsSDK.cmake"
        set(VSCOMN "VS90COMNTOOLS")
        string(LENGTH $ENV{${VSCOMN}} len)
        if(${len} EQUAL 0)
            set(VSCOMN "VS80COMNTOOLS")
            string(LENGTH $ENV{${VSCOMN}} len)
            if(${len} EQUAL 0)
                set(VSCOMN "VS70COMNTOOLS")
            endif(${len} EQUAL 0)
        endif(${len} EQUAL 0)
        execute_process(
            COMMAND ${ACMAKE_TOOLS_DIR}/VSvars.bat  "$ENV{${VSCOMN}}" "${CMAKE_BINARY_DIR}/VSVars.cmake"
            RESULT_VARIABLE rv
        )
    
    endif(WIN32)

    if (CMAKE_GENERATOR MATCHES "Visual Studio.*")
        option(ACMAKE_VERBOSE_VCPROJECTS
                "Print compiler and linker command lines (disable /nologo)?"
                ON)
        set(CMAKE_VERBOSE_MAKEFILE ${ACMAKE_VERBOSE_VCPROJECTS}
                CACHE BOOL
                "Print compiler and linker command lines (disable /nologo)?"
                FORCE)
    endif (CMAKE_GENERATOR MATCHES "Visual Studio.*")
endif(UNIX)

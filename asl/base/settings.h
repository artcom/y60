/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: settings.h,v $
//
//   $Revision: 1.19 $
//
// Description: all OS or Compiler dependent settings shall be defined here
//              as the capabilities and settings will change over time
//              ( There should be no "#if linux" or #if win32 settings elsewhere
//               than here!)
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
#ifndef _included_asl_settings_
#define _included_asl_settings_


/*! \addtogroup aslbase */
/* @{ */

/** @file
 * Compile time macro magic to detect compiler bugs and other platform dependent
 * diferences.
 */

// set to supress warnings about #undef'ing colliding symbols from system headers
#define _SETTING_NO_UNDEF_WARNING_ 1

#define WITH_TRACE_LOG
#if defined(DEBUG) || defined WITH_TRACE_LOG
#define _SETTING_WITH_TRACE_LOG_
#endif

#ifdef _GNU_SOURCE
    #if (__GNUC__ < 3)
        #define __SETTING_FORCE_STRSTREAM___
    #endif

    #if (__GNUC__ < 3)
        #define __SETTING_HAS_CHAR_TRAITS_NAMED_STRING_CHAR_TRAITS_
    #else
        #define __SETTING_HAS_CHAR_TRAITS_NAMED_CHAR_TRAITS
    #endif

    #ifdef __SETTING_FORCE_STRSTREAM___
        #define _SETTING_USE_STRSTREAM_INSTEAD_OF_STRINGSTREAM_
    #else
        #define _SETTING_USE_SGI_STRINGS_
    #endif
    #define _SETTING_USE_SYSVIPC_
    #define _SETTING_USE_MPFIFO_
    #define _SETTING_USE_PTHREAD_

    #define _SETTING_GCC_TEMPLATE_BUG_WORKAROUND_
#endif

#ifdef OSX
    #define _SETTING_GCC_TEMPLATE_BUG_WORKAROUND_
#endif

#ifdef _MSC_VER
// C4355 : 'this' : used in base member initializer list
// C4239 : nonstandard extension used : 'type cast' : conversion from '<temporary>' to '<non-const reference>'
// C4396: 'func' : the inline specifier cannot be used when a friend declaration refers to a specialization of a function template
#pragma warning( disable: 4355 /*4239*/ 4396 )
#endif // _MSC_VER

// this is not exactly accurate
#ifdef OSX_PPC
#   define _SETTING_BIG_ENDIAN_
#else
#   define _SETTING_LITTLE_ENDIAN_
#endif

/* @} */

namespace asl {
/*! \addtogroup aslbase */
/* @{ */
    class LittleEndianByteOrder{};
    typedef LittleEndianByteOrder X86ByteOrder;

    class BigEndianByteOrder {};
    typedef BigEndianByteOrder PowerPCByteOrder;

#ifdef _SETTING_LITTLE_ENDIAN_
    typedef X86ByteOrder AC_HOST_BYTE_ORDER;
#endif
#ifdef _SETTING_BIG_ENDIAN_
    typedef PowerPCByteOrder AC_HOST_BYTE_ORDER;
#endif

    typedef X86ByteOrder AC_DEFAULT_BYTE_ORDER;

    // the following typedefs may differ from arch to arch,
    // adapt as necessary
    typedef bool Boolean;
    typedef char Signed8;
    typedef unsigned char Unsigned8;
    typedef short Signed16;
    typedef unsigned short Unsigned16;
    typedef int Signed32;
    typedef unsigned int Unsigned32;
    typedef long long Signed64;
    typedef unsigned long long Unsigned64;
    typedef float Float32;
    typedef double Float64;
    typedef long double Float128;

#if 1    
    // 32-bit streams and blocks
    typedef Unsigned32 AC_SIZE_TYPE;
    typedef Signed32 AC_OFFSET_TYPE;
#else   
    // TODO: 64-bit streams and blocks
    // not ready yet - have to also think about
    // 64-bit file i/o
    typedef Unsigned32 AC_SIZE_TYPE;
    typedef Unsigned32 AC_OFFSET_TYPE;
#endif    

/* @} */
};

#ifdef WIN32
    // XXX: rename theDirectorySeparator to ourDirectorySeparator

    #define _SETTING_VC_TEMPLATE_BUG_WORKAROUND_

    // Debug symbols too long for std::map etc. in MSVC 6.0
    #define _SETTING_DISABLE_LONG_DEBUG_SYMBOL_WARNING_
    const char theDirectorySeparator = '\\';
#else
    const char theDirectorySeparator = '/';
#endif

#define _SETTING_PUT_FIELDS_INTO_SHARED_ARENA_

#ifdef LINUX
    #define HAVE_LONGLONG
    #define LONGLONG long long
#endif
#ifdef WIN32
    // prevent windows from defining min and max macros
    #define NOMINMAX
    #undef HAVE_LONGLONG
#endif

#ifndef LINUX
    #define _SETTING_USE_MUTEX_BASED_RWLOCK_IMPLEMENTATION_
#endif

#ifdef WIN32
    #define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

//#define PROFILING_LEVEL_NORMAL 1
//#define PROFILING_LEVEL_FULL 1


#define DESTRACE(x) //x

#if defined(WIN32) && defined(AC_BUILT_WITH_CMAKE)
#   pragma warning( disable:  4251)
#   define AC_DLL_EXPORT __declspec( dllexport )
#   define AC_DLL_IMPORT __declspec( dllimport )
#else
#   define AC_DLL_EXPORT
#   define AC_DLL_IMPORT
#endif


#endif // _included_asl_settings_


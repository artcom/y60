/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
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


// this is not exactly accurate
#ifdef OSX
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
    typedef unsigned long long AC_SIZE_TYPE;
    typedef signed long long AC_OFFSET_TYPE;
#endif    

/* @} */
};

#ifdef WIN32
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

#ifdef WIN32
	#include <crtdbg.h>
	// disable heap checking for debug builds by default, you must call this in main()
namespace asl {
/*! \addtogroup aslbase */
/* @{ */
	inline int disableHeapDebug(int theFlags = 0) {
#ifdef DEBUG_VARIANT
		return _CrtSetDbgFlag(theFlags);
#else
        return 0;
#endif
	}
/* @} */
} // namespace asl
#else
namespace asl {
/*! \addtogroup aslbase */
/* @{ */
		inline int disableHeapDebug(int theFlags = 0) {return 0;}
/* @} */
} // namespace asl
#endif

//#define PROFILING_LEVEL_NORMAL 1
//#define PROFILING_LEVEL_FULL 1

namespace asl {
	inline void initialize() {
		disableHeapDebug();
	}
} // namespace asl

#define DESTRACE(x) //x



#endif


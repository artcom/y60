/*
/--------------------------------------------------------------------
|
|      $Id: pldebug.h,v 1.8 2004/11/09 15:55:06 artcom Exp $
|
|      Plattform-independent support for PLASSERT_VALID, PLTRACE and
|      PLASSERT.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/
#ifndef INCL_PLDEBUG
#define INCL_PLDEBUG

#ifdef _DEBUG
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN  /* Prevent including <winsock*.h> in <windows.h> */
#define VC_EXTRALEAN  // Exclude rarely-used stuff from Windows headers
#include <windows.h>  // For OutputDebugString
#endif
#endif

//------------- PLASSERT_VALID
#ifdef _DEBUG
  #define PLASSERT_VALID(pOb) (pOb)->AssertValid()
#else
  #define PLASSERT_VALID(pOb) do{} while (0)
#endif

//------------- TRACE
#ifdef _DEBUG
  // Inlined to avoid differing linker signatures for debug and release
  // versions of paintlib.
  void PLTrace(const char * pszFormat, ...); 
  #define PLTRACE ::PLTrace
#else
  // This will be optimized away in release mode and still allow TRACE
  // to take a variable amount of arguments :-).
  inline void PLTrace (const char *, ...) { }
  #define PLTRACE  1 ? (void)0 : ::PLTrace
#endif

//------------- ASSERT

#ifdef _DEBUG
  #ifdef _WIN32
    #define PLASSERT(f)            \
      if (!(f))                    \
        {                          \
          PLTRACE ("Assertion failed at %s, %i\n", __FILE__, __LINE__); \
          __asm { int 3 }          \
        }
  #else
    #define PLASSERT(f)            \
      if (!(f))                    \
        {                          \
          PLTRACE ("Assertion failed at %s, %i\n", __FILE__, __LINE__); \
          abort();                 \
        }
  #endif
#else
  #define PLASSERT(f) do{}while (0)
#endif

#ifdef _DEBUG
  inline void PLTrace(const char * pszFormat, ...)
  {
    va_list args;
    va_start(args, pszFormat);

    int nBuf;
    char szBuffer[4096];

    nBuf = vsprintf(szBuffer, pszFormat, args);
    PLASSERT(nBuf < 4096);

#ifndef _WIN32
    fprintf (stderr, szBuffer);
#else
    ::OutputDebugString (szBuffer);
#endif //_WIN32

    va_end(args);
  }
#endif

//------------- CompilerAssert template for conditional
//              compile time error generation.

#define PLCOMPILER_ASSERT(cond) (void)sizeof(int[bool(cond)?1:-1]);


#endif // INCL_PLDEBUG

/*
/--------------------------------------------------------------------
|
|      $Log: pldebug.h,v $
|      Revision 1.8  2004/11/09 15:55:06  artcom
|      changed #ifdef _WINDOWS to #ifdef _WIN32 (since this is a macro that actually is predefined on  win32 platforms)
|
|      Revision 1.7  2004/09/11 12:41:35  uzadow
|      removed plstdpch.h
|
|      Revision 1.6  2004/06/13 20:19:27  uzadow
|      no message
|
|      Revision 1.5  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.4  2004/03/10 21:36:43  uzadow
|      pltester now has sensible output in non-debug mode.
|
|      Revision 1.3  2002/03/31 13:36:41  uzadow
|      Updated copyright.
|
|      Revision 1.2  2001/10/21 17:12:39  uzadow
|      Added PSD decoder beta, removed BPPWanted from all decoders, added PLFilterPixel.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.3  2000/12/04 23:55:40  uzadow
|      no message
|
|      Revision 1.2  2000/12/04 13:28:17  uzadow
|      Changed PLASSERT to use int 3 in windows builds.
|
|      Revision 1.1  2000/01/17 23:45:07  Ulrich von Zadow
|      MFC-Free version.
|
|
\--------------------------------------------------------------------
*/

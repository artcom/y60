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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef WIN32_BACKTRACE_INCLUDED
#define WIN32_BACKTRACE_INCLUDED

#include "asl_base_settings.h"

#include "TraceUtils.h"

#define _WINSOCKAPI_ // prevent winsock.h #include's

#include <crtdbg.h>
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Exclude rarely-used stuff from Windows headers
//#define WIN32_LEAN_AND_MEAN
//#define VC_EXTRALEAN

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif // _WIN32_WINNT
#ifndef WINVER
#define WINVER       0x0500
#endif // WINVER
#ifndef _WIN32_IE
#define _WIN32_IE	 0x0500
#endif // _WIN32_IE

#ifndef _M_IX86
#define UNICODE
#endif
#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE
#endif
#endif

#include <tchar.h>
#include <windows.h>

#if (WINVER < 0x0500)
#include <winable.h>
#endif // (WINVER < 0x0500)


#ifndef WT_EXECUTEINPERSISTENTIOTHREAD
#pragma message("You are not using the latest Platform SDK header/library ")
#pragma message("files. This may prevent the project from building correctly.")
#pragma message("You may install the Platform SDK from http://msdn.microsoft.com/downloads/")
#endif
//
#ifdef _DEBUG
#pragma warning(disable:4127)		// conditional expression is constant
#endif

#pragma warning(disable:4786)		// disable "identifier was truncated to 'number' characters in the debug information"
#pragma warning(disable:4290)		// C++ Exception Specification ignored
#pragma warning(disable:4097)		// typedef-name 'identifier1' used as synonym for class-name 'identifier2'
#pragma warning(disable:4001)		// nonstandard extension 'single line comment' was used
#pragma warning(disable:4100)		// unreferenced formal parameter
#pragma warning(disable:4699)		// Note: Creating precompiled header
#pragma warning(disable:4710)		// function not inlined
#pragma warning(disable:4514)		// unreferenced inline function has been removed
#pragma warning(disable:4512)		// assignment operator could not be generated
#pragma warning(disable:4310)		// cast truncates constant value



#include <eh.h>

#pragma warning(push, 3)
#include <iostream>
#include <sstream>
#include <string>
#include <exception>
#pragma warning(pop)


#include <stdio.h>

#include <imagehlp.h>
#include <ostream>
#include <vector>

#undef max
#undef RGB


namespace asl {

/** Policy to create backtraces on Win32 systems
 * It is used in conjunction with the StackTracer
 * template.
 */
class Win32Backtrace {
    public:
		struct Win32StackFrame : public StackFrameBase {
			std::string file;
			unsigned    line;
		};
		typedef Win32StackFrame StackFrame;

		static void trace(std::vector<StackFrame> & theStack, int theMaxDepth);

	private:
        Win32Backtrace();

		Win32Backtrace (unsigned);
		~Win32Backtrace();

		// stack walk
		bool stack_first (CONTEXT* pctx);
		bool stack_next  ();

		void address(unsigned a)		{ m_address = a; }
		unsigned address(void) const	{ return m_address; }

		// symbol handler queries
		unsigned symbol  (char *, unsigned, unsigned * = 0);
		unsigned fileline(char *, unsigned, unsigned *, unsigned * = 0);
		static bool get_line_from_addr (HANDLE, unsigned, unsigned *, IMAGEHLP_LINE *);

		static bool stack_trace(std::vector<StackFrame> & theStack, unsigned skip = 1);
		static bool stack_trace(Win32Backtrace&, std::vector<StackFrame> & theStack, CONTEXT *,
								unsigned skip = 1);
		bool check();

		unsigned		m_address;
		bool			m_ok;
		STACKFRAME *	m_pframe;
		CONTEXT *		m_pctx;

		class guard
		{
			private:
				guard();
			public:
				~guard();
				bool init();
				bool fail() const { return m_ref == -1; }

				static guard & instance() {
					static guard g;
					return g;
				}
			private:
				void clear();
				bool load_module(HANDLE, HMODULE);
				int  m_ref;
		};

};

}

#endif // WIN32_BACKTRACE_INCLUDED

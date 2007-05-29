//==============================================================================
//
// Copyright (C) 2007, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//==============================================================================

#ifndef WIN32_BACKTRACE_INCLUDED
#define WIN32_BACKTRACE_INCLUDED

#include "TraceUtils.h"

#include <crtdbg.h>
#if _MSC_VER > 1000 
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif // _WIN32_WINNT
#ifndef WINVER
#define WINVER       0x0500
#endif // WINVER
#ifndef _WIN32_IE
#define _WIN32_IE	 0x0500
#endif // _WIN32_IE

#include <tchar.h>
#include <windows.h>

#if (WINVER < 0x0500)
#include <winable.h>
#endif // (WINVER < 0x0500)


#ifdef _DEBUG
#pragma warning(disable:4127)		// conditional expression is constant
#endif

// windows is doing evil stuff
#undef max

#include <eh.h>
#pragma warning(push, 3) 
#include <iostream>
#include <string>
#include <exception>
#pragma warning(pop) 
#include <imagehlp.h>

#include <vector>



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

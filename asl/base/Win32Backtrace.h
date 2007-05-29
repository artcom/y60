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

#include "stdafx.h"
#include "TraceUtils.h"


#include <vector>
#include <string>


#include <imagehlp.h>
#include <ostream>


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

		// stack walk
		bool stack_first (CONTEXT* pctx);
		bool stack_next  ();

		void address(unsigned a)		{ m_address = a; }
		unsigned address(void) const	{ return m_address; }
	
		// symbol handler queries
		unsigned module  (char *, unsigned);
		unsigned symbol  (char *, unsigned, unsigned * = 0);
		unsigned fileline(char *, unsigned, unsigned *, unsigned * = 0);


		static void trace(std::vector<StackFrame> & theStack, int theMaxDepth);
		static bool stack_trace(std::vector<StackFrame> & theStack, unsigned skip = 1, const char * fmt = default_fmt()); 	
		
		static bool stack_trace(Win32Backtrace&, std::vector<StackFrame> & theStack, CONTEXT *, 
								unsigned skip = 1, const char * fmt = default_fmt());	
	
		static bool get_line_from_addr (HANDLE, unsigned, unsigned *, IMAGEHLP_LINE *);
		static unsigned get_module_basename (HMODULE, char *, unsigned);

		bool check();
    private:
        Win32Backtrace();
		Win32Backtrace (unsigned);
		~Win32Backtrace();

		unsigned		m_address;
		bool			m_ok;
		STACKFRAME *	m_pframe;
		CONTEXT *		m_pctx;
	
        static const char * default_fmt() { return "%f(%l) : %m at %s\n"; }	

		class guard
	{	
	private:
		guard();
	 public:
		~guard();
		bool init();		
		bool fail() const { return m_ref == -1; }
		static guard & instance() 
		{
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

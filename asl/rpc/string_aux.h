//
// string_aux.h
//
// This library is designated to act as glue between the ANSI-string
// type and CORBA-strings. It contains a set of functions and classes
// for server and client side string conversion in a unique manner.
// Use functions and classes beginning with "impl_" on the server
// side, and those with corresponding names on the client side.
// 
// This library (hopefully) eliminates most of the memory handling
// difficulties experienced by programmers using CORBA just from
// time to time.
//
// There is a performance penalty in some cases caused by multiple
// copying of the string, but I was not able to measure it because
// it is too small compared to all the other stuff going on there.
//
//  Another (theoretical) advantage of the scheme presented here is
//  that memory gets properly deallocated when an exception occurs,
//  and especially if you have premature returns in a function this
//  scheme is less error prone.
//
//  Pavel 1/99
//

#ifndef STRING_AUX_H
#define STRING_AUX_H

#include <OB/CORBA.h>
#include <string>
#include <strstream.h>


//-------------------------------------------------------------------
//
// Server Side String Helper classes
//

// The following classes can be used as parameter types
// instead of the standard CORBA string language binding types
// when using the tie-implementation; see below for conversion
// functions when deriving directly from _impl.


// The follwing examples illustrate the usage to wrap a function
// with this idl description:
//
//  string test(in string my_in,
//              inout string my_inout, 
//              out string my_out);
//

//---------------------------------------------
//
// Server side C++ example with tie approach:
//
//    impl_string test(impl_in_string my_in,
//			impl_inout_string my_inout,
//			impl_out_string my_out)
//    {
//	my_out.string = my_inout.string;
//	my_inout.string = my_in.string;
//	return impl_return_string("bla");
//    };

class impl_out_string;
class impl_inout_string;



//-------------------------------------------------------------------
//
// Client Side String Helper Functions
//
//  The following classes and function are the client side equivalents
//  of above server side functions.
//
//
//  Example:
//	string s1,s2,s3,s4;
//	s1 = return_string(object->test(in_string(s1),out_string(s2),inout_string(s3)));
//
//  Note: Some compilers destroy objects created in function calls
//	  at the end of the block and not at the end of the statement.
//	  In these cases you need to put the call into an extra block
//	  before you can access returned out or inout values.
//	{
//	s1 = return_string(object->test(in_string(s1),out_string(s2),inout_string(s3)));
//	}

//----------------------
//
//  in_string
//
//  this class ist just provided to have unique usage for
//  in, out and inout strings. You can use plain string
//  as well.
//

inline
const char* in_string(const std::string& s)
{
    return s.c_str();
};

//----------------------
//
//  inout_string
//
//  Use this class in method invocation for CORBA inout string
//  parameters only; you can not copy or assign this class.
//
//  How it works: On construction the string is copied into
//  a new CORBA string; a pointer to the constructing string is stored.
//  After returning from the call this object's destructor is called,
//  and the destructor copies whatever the CORBA-string now
//  contains into the original string and releases the CORBA-string.
//
class inout_string {
public:
    inout_string(std::string &s) {
	_stl=&s;
	_s = CORBA_string_dup(_stl->c_str());
	};
    operator char*&() {
	return _s;
    }
    ~inout_string() {
	*_stl = std::string(_s);
	CORBA_string_free(_s);
    }
    friend impl_inout_string;
private:
    inout_string() {};
    inout_string(const inout_string&) {};
protected:
    char** getrefptr() const {return const_cast<char**>(&_s);}
    char* _s;
    std::string* _stl;
};

//----------------------
//
//  impl_out_string
//
//  Use this class in method declaration for CORBA out string
//  parameters only; you can not copy or assign this class.
//
//  How it works: On construction a pointer to the constructing
//  string is stored.
//  After returning from the call this object's destructor is called,
//  and the destructor copies whatever the CORBA-string now
//  contains into the original string and releases the CORBA-string.
//
class out_string {
public:
    out_string(std::string &s) {
	_stl=&s;
    	_s = NULL;
    };
    operator char*&() {
	return _s;
    }
    ~out_string() {
	*_stl = std::string(_s);
	CORBA_string_free(_s);
    }
    friend impl_out_string;
private:
    out_string() {};
    out_string(const out_string&) {};
protected:
    char** getrefptr() const {return const_cast<char**>(&_s);}
    char* _s;
    std::string* _stl;
};

//----------------------
//
//  return_string
//
//  Use this function to wrap it around CORBA-Methods returning a string.
//  It copies the value into a ANSI string and releases the CORBA-string.
//

inline
std::string return_string(char *s) {
    std::string retval = s;
    CORBA_string_free(s);
    return retval;
};
//----------------------
//
//  impl_in_string
//
//  this class ist just provided to have unique usage for
//  in, out and inout strings. You can use plain string
//  as well.
//

class impl_in_string {
public:
    impl_in_string(const char* s) : string(s) {};
    std::string string;
};

//----------------------
//
//  impl_out_string
//
//  Use this class in method declaration for CORBA out string
//  parameters only; you can use the .string member in any way
//  you like, but you can not copy or assign this class.
//
//  How it works: On construction the pointer to the resultpointer
//  is stored; when leaving the method this object gets destroyed,
//  and the destructor copies whatever is in the string member into
//  a CORBA-string whose ownership is passed to the ORB.
//

class impl_out_string {
public:
    std::string string;    
public:
    impl_out_string(char*& s) : string("") {
	_s=&s;
    };
    impl_out_string(const out_string& s) : string("") {
	_s=s.getrefptr();
    };
    ~impl_out_string() {
	*_s = CORBA_string_dup(string.c_str());
    }
private:
    impl_out_string() {};
    impl_out_string(const impl_out_string&) {};
protected:
    char** _s;
};

//----------------------
//
//  impl_inout_string
//
//  Use this class in method declaration for CORBA inout string
//  parameters only; you can use the .string member in any way
//  you like, but you can not copy or assign this class.
//
//  How it works: On construction the CORBA-string is copied into
//  the string member of this class; the CORBA-string then is
//  destroyed and a pointer to the resultpointer is stored.
//  When leaving the method this object gets destroyed,
//  and the destructor copies whatever the string member now
//  contains into a CORBA-string whose ownership is passed to the ORB.
//

class impl_inout_string  {
public:
    std::string string;    
public:
    impl_inout_string(char*& s) : string(s)
    {
	CORBA_string_free(s);
	_s=&s;
    };
    impl_inout_string(const inout_string& s) : string("") {
	_s=s.getrefptr();
    };
    ~impl_inout_string() {
	*_s = CORBA_string_dup(string.c_str());
    }
private:
    impl_inout_string() {};
    impl_inout_string(const impl_inout_string&) {};
protected:
    char** _s;
};

//----------------------
//
// impl_return_string
//
// use impl_return_string to properly return any non-CORBA string
// as function return value; You can use it for returning string
// constants as well. (return impl_return_string("bla");)
//
inline
char* impl_return_string(const std::string& s)
{
    return(CORBA_string_dup(s.c_str()));
};

//----------------------
//
//  Use impl_string as return type for comfort and
//  readabilty.
//
typedef char* impl_string;

//---------------------------------------------
//
// Use the following conversion functions at the begin of
// the body of the server-side object implementation to convert
// from CORBA-string to ANSI string; this is not neccessary
// when using the tie-approach containers above.
//

inline
void impl_from_in_string(const char* p, std::string& s) {
    s = p;
};

inline
void impl_from_inout_string(char*& p, std::string& s) {
    s = p;
    CORBA_string_free(p);
};

//
// Use the following conversion function at the end of
// the body of the server-side object implementation to convert
// from ANSI-string to CORBA string; this is not neccessary
// when using the tie-approach containers above.
//

inline
void impl_to_out_string(const std::string& s, char*& r)
{
    r=CORBA_string_dup(s.c_str());
};

inline
void impl_to_inout_string(const std::string& s, char*& r)
{
    r=CORBA_string_dup(s.c_str());
};

//-------------------------------------------------------------------
//
//  This template function does not belong here and will be moved
//  somewhere else later. You can use it to convert anything with
//  defined stream output operator into an string.
//

template <class FROMTYPE>
std::string as_string(const FROMTYPE& t)
{
    ostrstream os;
    os << t;
    char *cp=os.str();
    std::string result = cp;
    free(cp);
    return result;
}
#endif

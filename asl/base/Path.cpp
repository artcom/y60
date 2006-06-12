/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description:  UTF-8 to locale encoding conversion class 
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Path.h"
#include "Assure.h"
#include "Logger.h"
#ifdef LINUX
#include <glib.h>
#endif


namespace asl {

Path::Path() {
#ifndef OSX
	_myLocaleChars = 0;
#endif
}

Path::Path(const char * theString, StringEncoding theEncoding)
{
#ifndef OSX
	_myLocaleChars = 0;
#endif
    assign(theString, theEncoding);
}

Path::Path(const std::string & theString, StringEncoding theEncoding)
{
#ifndef OSX
	_myLocaleChars = 0;
#endif
    assign(theString.c_str(), theEncoding);
}

Path::Path(const Path & theOther) {
#ifdef OSX
	_myString = CFStringCreateCopy(0, theOther._myString);
#else
	_myLocaleChars = 0;
    assign(theOther._myLocaleChars, Locale);
#endif
}

Path::~Path() {
    free();
}

void
Path::free() {
#ifdef WIN32
    if (_myLocaleChars) {
        ::free(_myLocaleChars);
    }
    _myLocaleChars = 0;
#elif LINUX
    if (_myLocaleChars) {
        g_free(_myLocaleChars);
    }
    _myLocaleChars = 0;
#endif
}


void
Path::assign(const char * theString, StringEncoding theEncoding) {
    free();

    switch (theEncoding) {
        case UTF8:
#ifdef OSX
			_myString = CFStringCreateWithCString(NULL, theString, kCFStringEncodingUTF8);
#elif WIN32
            {
                // convert from UTF8 to WideChars
                AC_SIZE_TYPE myWCharSize = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, theString, -1, 0, 0);
                LPWSTR myWChars = new WCHAR[myWCharSize];
                MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, theString, -1, myWChars, myWCharSize);

                // convert from WideChars to current codepage
                AC_SIZE_TYPE myLocaleSize = WideCharToMultiByte(CP_ACP, 0, myWChars, -1, 0, 0, 0, 0);
                _myLocaleChars = static_cast<char *>(malloc(myLocaleSize));
                WideCharToMultiByte(CP_ACP, 0, myWChars, -1, _myLocaleChars, myLocaleSize, 0, 0);

                delete [] myWChars;
            }
#else
            _myLocaleChars = g_filename_from_utf8(theString, -1, 0, 0, 0);
#endif
            break;
        case Locale:
#ifdef OSX
			_myString = CFStringCreateWithCString(NULL, theString, CFStringGetSystemEncoding());
#elif WIN32
            _myLocaleChars = _strdup(theString);
#else
            _myLocaleChars = g_strdup(theString); 
#endif
            break;
    }
}
    
std::string
Path::toLocale() const {
#ifdef OSX
	CFRange myRange = CFRangeMake(0, CFStringGetLength(_myString));
	CFIndex myBufferSize = 0;
	std::string myResult;
	CFIndex myCharCount = CFStringGetBytes(_myString, myRange, CFStringGetSystemEncoding(), '?', FALSE, 0, 0, &myBufferSize);
	if (myCharCount) {
		myResult.resize(myBufferSize);
		CFIndex myExCharCount = CFStringGetBytes(_myString, myRange, CFStringGetSystemEncoding(), '?',
													FALSE, (UInt8*)&(*(myResult.begin())), myBufferSize, &myBufferSize);
		ASSURE_WITH(AssurePolicy::Throw, myExCharCount == myCharCount);
	}
	return myResult;
#else    
    if (_myLocaleChars == 0) {
        return std::string();
    }
    return std::string(_myLocaleChars);
#endif
}

std::string 
Path::toUTF8() const {
#ifdef OSX
	CFRange myRange = CFRangeMake(0, CFStringGetLength(_myString));
	CFIndex myBufferSize = 0;
	std::string myResult;
	CFIndex myCharCount = CFStringGetBytes(_myString, myRange, kCFStringEncodingUTF8, '?', FALSE, 0, 0, &myBufferSize);
	if (myCharCount) {
		myResult.resize(myBufferSize);
		CFIndex myExCharCount = CFStringGetBytes(_myString, myRange, kCFStringEncodingUTF8, '?',
													FALSE, (UInt8*)&(*(myResult.begin())), myBufferSize, &myBufferSize);
		ASSURE_WITH(AssurePolicy::Throw, myExCharCount == myCharCount);
	}
	return myResult;
#elif WIN32
    // TODO: convert locale to UTF8
    if (_myLocaleChars == 0) {
        return std::string();
    }
    // convert from active codepage to WideChars
    AC_SIZE_TYPE myWCharSize = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, _myLocaleChars, -1, 0, 0);
    LPWSTR myWChars = new WCHAR[myWCharSize];
    MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, _myLocaleChars, -1, myWChars, myWCharSize);

    // convert from WideChars to UTF8
    AC_SIZE_TYPE myUTF8Size = WideCharToMultiByte(CP_UTF8, 0, myWChars, -1, 0, 0, 0, 0);
    char * myUTF8Chars = new char[myUTF8Size];
    WideCharToMultiByte(CP_UTF8, 0, myWChars, -1, myUTF8Chars, myUTF8Size, 0, 0);
    
    std::string myUTF8String(myUTF8Chars);
    delete [] myWChars;
    delete [] myUTF8Chars;
    return myUTF8String;
#else
    if (_myLocaleChars == 0) {
        return std::string();
    }
    gchar * myUTF = g_filename_to_utf8(_myLocaleChars, -1, 0, 0, 0);
    if ( ! myUTF) {
        throw asl::Exception(std::string("Failed to convert filename '") + _myLocaleChars  +
                "' to UTF8.", PLUS_FILE_LINE);
    }
    std::string myUTF8String(myUTF);
    g_free(myUTF);
   return myUTF8String;
 #endif
}


Path & 
Path::operator=(const Path& theOther) {
	if (this == &theOther) {
		return *this;
	}
#ifdef OSX
	_myString = CFStringCreateCopy(0, theOther._myString);
#else
    assign(theOther._myLocaleChars, Locale);
#endif
	return *this;
}

bool
Path::empty() const {
#ifdef OSX
 return CFStringGetLength(_myString) == 0;
#else
    return _myLocaleChars == 0 || strlen(_myLocaleChars) == 0;
#endif
}

bool
Path::operator==(const Path& theOther) const {
#ifdef OSX
	return CFStringCompare(_myString, theOther._myString, 0) == kCFCompareEqualTo;
#else
	if (this->_myLocaleChars == theOther._myLocaleChars) {
		return true;
	}
    return strcmp(this->_myLocaleChars, theOther._myLocaleChars) == 0;
#endif
}

std::ostream & operator << (std::ostream & os, const asl::Path & thePath) {
    return os << thePath.toLocale();
}


}


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
// Description:  UTF-8 to locale encoding conversion class
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

// own header
#include "Path.h"

#include "Assure.h"
#include "Logger.h"
#include "error_functions.h"
#ifdef LINUX
#include <glib.h>
#endif

#ifdef _WIN32

BOOL IsWin2kSP4OrLater()
{
   OSVERSIONINFOEX osvi;
   DWORDLONG dwlConditionMask = 0;
   BYTE op=VER_GREATER_EQUAL;

   // Initialize the OSVERSIONINFOEX structure.

   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
   osvi.dwMajorVersion = 5;
   osvi.dwMinorVersion = 0;
   osvi.wServicePackMajor = 4;
   osvi.wServicePackMinor = 0;

   // Initialize the condition mask.

   VER_SET_CONDITION( dwlConditionMask, VER_MAJORVERSION, op );
   VER_SET_CONDITION( dwlConditionMask, VER_MINORVERSION, op );
   VER_SET_CONDITION( dwlConditionMask, VER_SERVICEPACKMAJOR, op );
   VER_SET_CONDITION( dwlConditionMask, VER_SERVICEPACKMINOR, op );

   // Perform the test.

   return VerifyVersionInfo(
      &osvi,
      VER_MAJORVERSION | VER_MINORVERSION |
      VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
      dwlConditionMask);
}
int getMultibyteToWideCharFlags() {
    static int myFlags = IsWin2kSP4OrLater() ? MB_ERR_INVALID_CHARS : 0;
    return myFlags;
}
#endif

namespace asl {

DEFINE_EXCEPTION(UnicodeException, Exception);

Path::Path() {
#ifdef OSX
    _myString = 0;
#else
	_myLocaleChars = 0;
#endif
}

Path::Path(const char * theString, StringEncoding theEncoding)
{
#ifdef OSX
    _myString = 0;
#else
	_myLocaleChars = 0;
#endif
    assign(theString, theEncoding);
}

Path::Path(const std::string & theString, StringEncoding theEncoding)
{
#ifdef OSX
    _myString = 0;
#else
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
#ifdef _WIN32
    if (_myLocaleChars) {
        ::free(_myLocaleChars);
    }
    _myLocaleChars = 0;
#elif OSX
    if (_myString) {
        CFRelease(_myString);
    }
    _myString = 0;
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
#elif _WIN32
            {
                // convert from UTF8 to WideChars
                AC_SIZE_TYPE myWCharSize = MultiByteToWideChar(CP_UTF8, getMultibyteToWideCharFlags(), theString, -1, 0, 0);
                if (myWCharSize == 0) {
                    throw UnicodeException(errorDescription(lastError()), PLUS_FILE_LINE);
                }
                LPWSTR myWChars = new WCHAR[myWCharSize];
                MultiByteToWideChar(CP_UTF8, getMultibyteToWideCharFlags(), theString, -1, myWChars, myWCharSize);

                // convert from WideChars to current codepage
                AC_SIZE_TYPE myLocaleSize = WideCharToMultiByte(CP_ACP, 0, myWChars, -1, 0, 0, 0, 0);
                if (myLocaleSize == 0) {
                    throw UnicodeException(errorDescription(lastError()), PLUS_FILE_LINE);
                }
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
#elif _WIN32
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
#elif _WIN32
    if (_myLocaleChars == 0) {
        return std::string();
    }
    // convert from active codepage to WideChars
    AC_SIZE_TYPE myWCharSize = MultiByteToWideChar(CP_ACP, getMultibyteToWideCharFlags(), _myLocaleChars, -1, 0, 0);
    if (myWCharSize == 0) {
        throw UnicodeException(errorDescription(lastError()), PLUS_FILE_LINE);
    }
    LPWSTR myWChars = new WCHAR[myWCharSize];
    MultiByteToWideChar(CP_ACP, getMultibyteToWideCharFlags(), _myLocaleChars, -1, myWChars, myWCharSize);

    // convert from WideChars to UTF8
    AC_SIZE_TYPE myUTF8Size = WideCharToMultiByte(CP_UTF8, 0, myWChars, -1, 0, 0, 0, 0);
    if (myUTF8Size == 0) {
        throw UnicodeException(errorDescription(lastError()), PLUS_FILE_LINE);
    }
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
        throw UnicodeException(std::string("Failed to convert filename '") + _myLocaleChars  +
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


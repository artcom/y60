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

#include "Logger.h"
#include <glib.h>

namespace asl {

Path::Path() : _myLocaleChars(0)
{
}

Path::Path(const char * theString, StringEncoding theEncoding) : _myLocaleChars(0)
{
    assign(theString, theEncoding);
}

Path::Path(const std::string & theString, StringEncoding theEncoding) : _myLocaleChars(0)
{
    assign(theString.c_str(), theEncoding);
}

Path::Path(const Path & theOther) : _myLocaleChars(0) {
    assign(theOther._myLocaleChars, Locale);
}

Path::~Path() {
    if (_myLocaleChars) {
        g_free(_myLocaleChars);
        _myLocaleChars = 0;
    }
}

void
Path::assign(const char * theString, StringEncoding theEncoding) {
    if (_myLocaleChars) {
        g_free(_myLocaleChars);
        _myLocaleChars = 0;
    }
    switch (theEncoding) {
        case UTF8:
            _myLocaleChars = g_filename_from_utf8(theString, -1, 0, 0, 0);
            break;
        case Locale:
            _myLocaleChars = g_strdup(theString); 
            break;
    }
}
    
std::string
Path::toLocale() const {
    if (_myLocaleChars == 0) {
        return std::string();
    }
    return std::string(_myLocaleChars);
}

std::string 
Path::toUTF8() const {
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
}


Path & 
Path::operator=(const Path& theOther) {
	if (this == &theOther) {
		return *this;
	}
    assign(theOther._myLocaleChars, Locale);
	return *this;
}

bool
Path::empty() const {
    return _myLocaleChars == 0 || strlen(_myLocaleChars) == 0;
}

bool
Path::operator==(const Path& theOther) const {
	if (this->_myLocaleChars == theOther._myLocaleChars) {
		return true;
	}
    return strcmp(this->_myLocaleChars, theOther._myLocaleChars) == 0;
}

std::ostream & operator << (std::ostream & os, const asl::Path & thePath) {
    return os << thePath.toLocale();
}


}


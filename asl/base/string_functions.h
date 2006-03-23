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
//    $RCSfile: string_functions.h,v $
//
//   $Revision: 1.16 $
//
// Description: string helper functions
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
#ifndef _included_asl_string_functions_
#define _included_asl_string_functions_

#include "settings.h"

#include <string>
#include <vector>
#include <ctype.h>

#ifdef _SETTING_USE_STRSTREAM_INSTEAD_OF_STRINGSTREAM_
#  include <strstream.h>
#else
#  include <sstream>
#endif
#include "Exception.h"

namespace asl {


    /*! \addtogroup aslbase */
    /* @{ */

    extern const char cb64[];
    extern const char cb66[];
    extern const char cb67[];

    enum StringEncoding {
        Locale,
        UTF8
    };
    
    DEFINE_EXCEPTION(BufferTooSmall, asl::Exception);

    /// return every type that has a operator>>(ostream) as string
#ifdef _SETTING_USE_STRSTREAM_INSTEAD_OF_STRINGSTREAM_
    template <class T>
    std::string as_string(const T& x) {
        std::ostrstream streamer;
        //streamer << x  << std::string::traits_type::eos();
        streamer << x << '\0';

        // XXX avoid Memory leak after .str()
    	char * result_str = streamer.str();
	    std::string result = result_str;
	    delete result_str;
        return result;
    }

    template <class T>
    bool fromString(const std::string & theString, T & outValue) {
        istrstream myStream(theString.c_str());
        return myStream >> outValue;
    }

#else
    template <class T>
    std::string as_string(const T& x) {
        std::ostringstream streamer;
        streamer << x;
        return streamer.str();
    }

    template <class T>
    bool fromString(const std::string & theString, T & outValue) {
        std::istringstream myStream(theString);
        myStream >> outValue;
        return (myStream != 0); // Shouldn't this be (myStream.good() || myStream.eof()) ? (MS)
    }
#endif


    bool fromString(const std::string & theString, bool & outValue);

    inline
    bool fromString(const std::string & theString, std::string & outValue) {
        outValue = theString;
        return true;
    };




    template <class T>
    T
    as(const std::string & theString) {
        T outValue;
        if (!asl::fromString(theString,outValue)) {
            throw ParseException("as","could not convert to desired type");
        };
        return outValue;
    }

    int as_int(const std::string & theString);

    inline bool is_space(char c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }
    inline bool is_alpha(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }
    inline bool is_printable(char c) {
        return (c >= 0x20 && c < 0x7f);
    }
    inline bool is_underscore(char c) {
        return c == '_';
    }
    inline bool is_colon(char c) {
        return c == ':';
    }
    inline bool is_dot(char c) {
        return c == '.';
    }
    inline bool is_comma(char c) {
        return c == ',';
    }
    inline bool is_hyphen(char c) {
        return c == '-';
    }
    inline bool is_digit(char c) {
        return c >= '0' && c <= '9';
    }
    inline bool is_ABCDEF(char c) {
        return c >= 'A' && c <= 'F';
    }
    inline bool is_abcdef(char c) {
        return c >= 'a' && c <= 'f';
    }
    inline unsigned int digit_to_num(char digit) {
        return digit - '0';
    }
    inline unsigned int ABCDEF_to_num(char digit) {
        return digit - 'A' + 10;
    }
    inline unsigned int abcdef_to_num(char digit) {
        return digit - 'a' + 10;
    }

    /// convert a ascii hex digit to its binary value. ('2' -> 2, 'A' -> 10)
    /// returns false if not a hex digit
    bool hex_to_num(char digit, unsigned int & num);

    void binToString(unsigned char * theData, unsigned int theLength, std::string & theDest);
    bool stringToBin(const std::string & theString, unsigned char * theData, unsigned int maxLength);

	std::vector<std::string> splitString(const std::string & theString, const std::string & theSeparators = " \n\r\t");

    // Converts an enum into a string, given a string list with the same layout as the enum
    // and a zero at the end
    template<class ENUM>
    const char * getStringFromEnum(const ENUM & theEnum, const char * theStringList[]) {
        int myCounter = -1;
        // Find number of strings in string list
        while (theStringList[++myCounter]) {
        }
        if (theEnum >= myCounter) {
            throw ParseException(std::string("Could not find enum '") + as_string<int>(theEnum) + "' in string list",
                                 "getStringFromEnum()");
        }

        return theStringList[static_cast<unsigned>(theEnum)];
    }

    // Converts a string into an enum, given a string list with the same layout as the enum
    // and a zero at the end
    int getEnumFromString(const std::string & theString, const char * theStringList[]);

    void findAndReplace(std::string & theString, const char * theSearchString, const char * theReplaceString);
    std::string foundAndReplaced(const std::string & theSource,
                                 const std::string & theSearchString,
                                 const std::string & theReplaceString);

    inline std::string trimLeft (const std::string & theString, const std::string & theTrimChars = " " ) {
        std::string myString(theString);
        return myString.erase(0, theString.find_first_not_of ( theTrimChars ));
    }

    inline std::string trimRight (const std::string & theString, const std::string & theTrimChars = " ") {
        std::string myString(theString);
        return myString.erase(theString.find_last_not_of ( theTrimChars ) + 1);
    }

    inline std::string trim (const std::string & theString, const std::string & theTrimChars = " ") {
        std::string myString(theString);
        return trimLeft ( trimRight ( myString, theTrimChars), theTrimChars);
    }

    inline std::string toUpperCase (const std::string & theString) {
        std::string myString;
        myString.resize(theString.size());
        for (unsigned i = 0; i < theString.size(); ++i) {
            myString[i] = ::toupper(theString[i]);
        }
        return myString;
    }

    inline std::string toLowerCase (const std::string & theString) {
        std::string myString;
        myString.resize(theString.size());
        for (unsigned i = 0; i < theString.size(); ++i) {
            myString[i] = ::tolower(theString[i]);
        }
        return myString;
    }

    void binToBase64(const unsigned char * theData, unsigned int theLength, std::string & theDest, const char * theCodeTable = cb66);
    unsigned base64ToBin(const std::string & theSource, unsigned char * theDest, unsigned int maxLength);

    /* @} */

} //Namespace asl

#endif


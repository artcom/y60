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

#include "asl_base_settings.h"

#include "settings.h"

#include <string>
#include <vector>
#include <ctype.h>

#ifdef _WIN32
#   include <wtypes.h> // Platform SDK (for BSTR typedef)
#endif

#ifdef _SETTING_USE_STRSTREAM_INSTEAD_OF_STRINGSTREAM_
#  include <strstream.h>
#else
#  include <sstream>
#endif

#include "Exception.h"

namespace asl {

    /*! \addtogroup aslbase */
    /* @{ */

    const char VECTOR_OF_STRING_DELIMITER = '`';

    typedef unsigned char Char;
    const int MAX_CHAR_VALUE = 255 * 16;

    ASL_BASE_DECL extern const Char cb64[];
    ASL_BASE_DECL extern const Char cb66[];
    ASL_BASE_DECL extern const Char cb67[];

    enum StringEncoding {
        Locale,
        UTF8
    };

    DEFINE_EXCEPTION(BufferTooSmall, asl::Exception);

#ifdef _SETTING_FLOAT_ISTREAM_UNDERFLOW_WORKAROUND_    
    ASL_BASE_DECL bool fromString(const std::string & theString, float & outValue);
    ASL_BASE_DECL bool fromString(const std::string & theString, double & outValue);
#endif
    /// return every type that has a operator>>(ostream) as string
#ifdef _SETTING_USE_STRSTREAM_INSTEAD_OF_STRINGSTREAM_
    template <class T>
    inline std::string as_string(const T& x) {
        std::ostrstream streamer;
        //streamer << x  << std::string::traits_type::eos();
        streamer << x << '\0';

        // XXX avoid Memory leak after .str()
    	Char * result_str = streamer.str();
	    std::string result = result_str;
	    delete result_str;
        return result;
    }

    template <class T>
    inline bool fromString(const std::string & theString, T & outValue) {
        istrstream myStream(theString.c_str());
        return myStream >> outValue;
    }

#else
    template <class T>
    inline std::string as_string(const T& x) {
        std::ostringstream streamer;
        streamer << x;
        return streamer.str();
    }

    template <class T>
    inline bool fromString(const std::string & theString, T & outValue) {
        std::istringstream myStream(theString);
        myStream >> outValue;
        return (myStream != 0); // Shouldn't this be (myStream.good() || myStream.eof()) ? (MS)
                                // PM: it would be the same, stream::operator void*() is called here which
                                // return 0 whhen neither the fail nor the bad bit is set, and ignores eofbit
                                // while good() considers all three existing error flagss.
    }
#endif


    ASL_BASE_DECL bool fromString(const std::string & theString, bool & outValue);

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
            throw ParseException("as<T>",std::string("could not convert '")+theString+"' to type "+demangled_name<T>());
        };
        return outValue;
    }

    ASL_BASE_DECL int as_int(const std::string & theString);

    inline bool is_space(Char c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }
    inline bool is_alpha(Char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }
    inline bool is_printable(Char c) {
        return (c >= 0x20 && c < 0x7f);
    }
    inline bool is_underscore(Char c) {
        return c == '_';
    }
    inline bool is_colon(Char c) {
        return c == ':';
    }
    inline bool is_dot(Char c) {
        return c == '.';
    }
    inline bool is_comma(Char c) {
        return c == ',';
    }
    inline bool is_hyphen(Char c) {
        return c == '-';
    }
    inline bool is_digit(Char c) {
        return c >= '0' && c <= '9';
    }
    inline bool is_ABCDEF(Char c) {
        return c >= 'A' && c <= 'F';
    }
    inline bool is_abcdef(Char c) {
        return c >= 'a' && c <= 'f';
    }
    inline unsigned int digit_to_num(Char digit) {
        return digit - '0';
    }
    inline unsigned int ABCDEF_to_num(Char digit) {
        return digit - 'A' + 10;
    }
    inline unsigned int abcdef_to_num(Char digit) {
        return digit - 'a' + 10;
    }

    /// convert a ascii hex digit to its binary value. ('2' -> 2, 'A' -> 10)
    /// returns false if not a hex digit
    bool hex_to_num(Char digit, unsigned int & num);

    ASL_BASE_DECL void binToString(unsigned char * theData, unsigned int theLength, std::string & theDest);
    ASL_BASE_DECL bool stringToBin(const std::string & theString, unsigned char * theData, unsigned int maxLength);

    ASL_BASE_DECL std::vector<std::string> splitString(const std::string & theString, const std::string & theSeparators = " \n\r\t");

    // Converts an enum into a string, given a string list with the same layout as the enum
    // and a zero at the end
    template<class ENUM>
    const char * getStringFromEnum(const ENUM & theEnum, const char * const theStringList[]) {
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
    ASL_BASE_DECL int getEnumFromString(const std::string & theString, const char * const theStringList[]);

    ASL_BASE_DECL void findAndReplace(std::string & theString, const char * theSearchString, const char * theReplaceString);
    ASL_BASE_DECL std::string foundAndReplaced(const std::string & theSource,
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
            myString[i] = static_cast<char>(::toupper(theString[i]));
        }
        return myString;
    }

    inline std::string toLowerCase (const std::string & theString) {
        std::string myString;
        myString.resize(theString.size());
        for (unsigned i = 0; i < theString.size(); ++i) {
            myString[i] = static_cast<char>(::tolower(theString[i]));
        }
        return myString;
    }

    ASL_BASE_DECL void binToBase64(const unsigned char * theData, unsigned int theLength, std::string & theDest, const Char * theCodeTable = cb66);
    ASL_BASE_DECL unsigned base64ToBin(const std::string & theSource, unsigned char * theDest, unsigned int maxLength);

    // Returns the line in theString starting at theCurPos and ending in \n.
    // After the call, theCurPos points to the start of the next line in the
    // string.
    // Rudimentary, but very helpful when parsing line-based files.
    std::string getNextLine(std::string::size_type& theCurPos, const std::string& theString);

#ifdef _WIN32
    ASL_BASE_DECL char* convertBSTRToLPSTR(BSTR theBSTR);
    ASL_BASE_DECL char* convertLPWSTRToLPSTR(LPWSTR theLPWSTR);
#endif

    inline bool is_ascii(Char c) {
        return (c <= 0x7f);
    }
    inline bool is_utf8_sequence_followup(Char c) {
        return (c >= 0x80) && (c <= 0xBF);
    }
    inline bool is_utf8_sequence_start(Char c) {
        return (c & 0xC0) == 0xC0;
    }
    inline bool is_utf8_multibyte(Char c) {
        return is_utf8_sequence_start(c) || is_utf8_sequence_followup(c);
    }

   /*
    datachar    ::= '&amp;' | '&lt;' | '&gt;' | '&quot;'
    | '&#9;'| '&#10;'| '&#13;'
    | (char - ('&' | '<' | '>' | '"' | #x9 | #xA | #xD))

    */

    /// convert a ascii hex digit to its binary value. ('2' -> 2, 'A' -> 10)
    /// returns false if not a hex digit
    inline bool hex_to_num(Char digit, unsigned int & num) {
        if (is_digit(digit)) {
            num = digit_to_num(digit);
            return true;
        } else if (is_ABCDEF(digit)) {
            num = ABCDEF_to_num(digit);
            return true;
        } else if (is_abcdef(digit)) {
            num = abcdef_to_num(digit);
            return true;
        }
        return false;
    }

    inline bool is_decimal_number(const std::string & s, unsigned int & r, unsigned int max_value = MAX_CHAR_VALUE) {
        if (s.size() == 0)
            return false;
        unsigned int result = 0;
        for (unsigned int i = 0; i< s.size();++i) {
            if (is_digit(s[i])) {
                result*=10;
                result+=digit_to_num(s[i]);
            } else
                return false;
            if (result > max_value)
                return false;
        }
        r = result;
        return true;
    }

    inline bool is_hex_number(const std::string & s, unsigned int & r, unsigned int max_value = MAX_CHAR_VALUE) {
        if (s.size() == 0)
            return false;
        unsigned int result = 0;
        for (unsigned int i = 0; i< s.size();++i) {
            unsigned int num;
            if (hex_to_num(s[i],num)) {
                result*=16;
                result+=num;
            } else
                return false;
            if (result > max_value)
                return false;
        }
        r = result;
        return true;
    }

    // returns the position of the first non-whitespace char
    inline std::string::size_type read_whitespace(const std::string & is, std::string::size_type pos) {
        while (pos < is.size() && (is_space(is[pos]))) {
            ++pos;
        }
        return pos;
    }

    inline bool is_name_start_char(Char c) {
        return is_alpha(c) || is_underscore(c) || is_colon(c);
    }
    inline bool is_name_char(Char c) {
        return is_name_start_char(c) || is_digit(c) || is_dot(c) || is_hyphen(c);
    }

    inline bool is_identifier_start_char(Char c) {
        return is_alpha(c) || is_underscore(c);
    }
    inline bool is_identifier_char(Char c) {
        return is_identifier_start_char(c) || is_digit(c);
    }

    template< typename StartCharFunc, typename CharFunc >
    inline std::string::size_type read_object( const std::string& is
                                             , std::string::size_type pos
                                             , StartCharFunc is_start_char_func
                                             , CharFunc is_char_func) {
        if ( is_start_char_func(is[pos]) && pos < is.size() ) {
            ++pos;
            while (is_char_func(is[pos]) && pos < is.size() ) {
                ++pos;
            }
        }
        return pos;
    }

    // returns the position of the first non-namechar
    inline std::string::size_type read_name(const std::string& is,std::string::size_type pos) {
        return read_object(is,pos,is_name_start_char,is_name_char);
    }

    // returns the position of the first non-identifier char
    inline std::string::size_type read_identifier(const std::string& is,std::string::size_type pos) {
        return read_object(is,pos,is_identifier_start_char,is_identifier_char);
    }

    // returns pos + 1 if c is at pos
    inline std::string::size_type read_if_char(const std::string & is,std::string::size_type pos, Char c) {
        if ( is[pos] == c && pos < is.size() ) {
            return pos + 1;
        }
        return pos;
    }

    // returns position past end of s if s is the exact next part
    inline std::string::size_type read_if_string(const std::string & is,std::string::size_type pos, const std::string & s) {
        std::string::size_type i = 0;
        std::string::size_type n = s.size()<is.size()-pos?s.size():is.size()-pos;
        while (i<n && pos < is.size() && is[pos+i] == s[i]) {
            ++i;
        }
        if (i == s.size())
            return pos + i;
        return pos;
    }

    // returns pos of next delim
    inline std::string::size_type read_text(const std::string & is,std::string::size_type pos, Char delim) {
        std::string::size_type last_non_white = pos;
        while ( pos < is.size() && is[pos] != delim ) {
            if (!is_space(is[pos])) last_non_white = pos;
            ++pos;
        }
        return last_non_white+1;
    }
    // returns position past second quote char if at pos is a quote char
    inline std::string::size_type read_quoted_text(const std::string & is,std::string::size_type pos,
        Char opening_qoute,Char closing_qoute ) {
        if (is[pos]==opening_qoute) {
            ++pos;
            while ( pos < is.size() && is[pos] != closing_qoute ) {
                ++pos;
            }
            if (is[pos]==closing_qoute) ++pos;
        }
        return pos;
    }

    inline void copy_between_quotes(const std::string& is,std::string::size_type pos, std::string::size_type end_pos, std::string & dest) {
        dest=is.substr(pos+1,end_pos-pos-2);
    }
    // returns position past "right" if sequence starts with "left"
    inline std::string::size_type read_if_between(const std::string & is,std::string::size_type pos, const std::string & left, const std::string & right)
    {
        std::string::size_type left_end = read_if_string(is,pos,left);
        if (left_end > pos) {
            std::string::size_type right_begin = is.find(right,left_end);
            if (right_begin != std::string::npos)
                pos = right_begin + right.size();
        }
        return pos;
    }

    // utility for previous function to copy what's between into string dest
    inline void copy_between(const std::string & is,std::string::size_type pos, const std::string & left, const std::string & right,
        std::string::size_type right_end_pos, std::string & dest)
    {
        std::string::size_type begin = pos+left.size();
        dest = is.substr(begin,right_end_pos-begin-right.size());
    }


    /* @} */

} //Namespace asl

#endif


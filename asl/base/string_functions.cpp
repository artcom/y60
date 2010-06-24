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
//    $RCSfile: string_functions.cpp,v $
//
//   $Revision: 1.9 $
//
// Description: string helper functions
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

// own header
#include "string_functions.h"

#include <cstring>
#include <errno.h>
#include <stdlib.h>
#include <math.h>

using namespace std;

namespace asl {

#define DB(x) // x

    int as_int(const string & theString) {
        int outValue;
#ifdef _SETTING_USE_STRSTREAM_INSTEAD_OF_STRINGSTREAM_
        istrstream myStream(theString.c_str());
#else
        istringstream myStream(theString);
#endif
        myStream >> outValue;
        if (!myStream) {
            throw ParseException("as_int","could not convert to integer");
        };
        return outValue;
    }

    bool fromString(const string & theString, bool & outValue) {
        if (theString == "true" || theString=="1") {
            outValue = true;
            return true;
        }
        if (theString == "false" || theString=="0") {
            outValue = false;
            return true;
        }
        return false;
    }

#ifdef _SETTING_FLOAT_ISTREAM_UNDERFLOW_WORKAROUND_
    bool fromString(const std::string & theString, float & outValue) {
        errno = 0;
        char * end = 0;
#ifdef _SETTING_NO_STRTOF_     
        outValue = static_cast<float>(strtod(theString.c_str(), &end));
#else
        outValue = strtof(theString.c_str(), &end);
#endif
		// if errno = ERANGE, then we have an over- or underflow.
		// if its an overflow, outValue will be very big (positive or negative infinity)
		// if its an underflow, outValue will be almost zero
        if (errno == ERANGE && fabsf(outValue) < 1.0f) { 
          return true; // underflow
        }
        if (errno != 0) {
            return false; // some other error while parsing
        }
        if (end && *end !='\0') { // trailing chars
            return false;
        }
        return true; // all ok
    }

    bool fromString(const std::string & theString, double & outValue) {
        errno = 0;
        char * end = 0;
        outValue = strtod(theString.c_str(), &end);

        if (errno == ERANGE && fabs(outValue) < 1.0) { 
          return true; // underflow
        }
        if (errno != 0) {
            return false; // some other error while parsing
        }
        if (end && *end !='\0') { // trailing chars
            return false;
        }
        return true; // all ok
    }
#endif

    bool
    hex_to_num(char digit, unsigned int & num) {
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

    void
    binToString(unsigned char * theData, unsigned int theLength, string & theDest) {
        static const char * hexString = "0123456789ABCDEF";
        theDest.resize(2*theLength);
        for (unsigned int i=0; i<theLength; i++) {
            int upperNibble = theData[i] >> 4;
            int lowerNibble = theData[i] & 0x0f;
            theDest[i*2] = hexString[upperNibble];
            theDest[i*2+1] = hexString[lowerNibble];
        }
    }

    bool
    stringToBin(const string & theString, unsigned char * theData, unsigned int maxLength) {
        for (unsigned int i=0; i< theString.size()/2; i++) {
            if (i >= maxLength) {
                return false;
            }

            unsigned int upperNibble, lowerNibble;
            if (!hex_to_num(theString[i*2], upperNibble)) {
                return false;
            }

            if (!hex_to_num(theString[i*2+1], lowerNibble)) {
                return false;
            }
            theData[i] = static_cast<unsigned char>(upperNibble << 4 | lowerNibble);
        }
        return true;
    }

    std::vector<std::string> splitString(const std::string & theString, const std::string & theSeparators) {
        std::vector<std::string> theResult;
        theResult.push_back("");
        for (std::string::size_type i = 0; i < theString.size();++i) {
            if (theSeparators.find(theString[i]) == std::string::npos) {
                theResult.back().push_back(theString[i]);
            } else {
                if (theResult.back().size() > 0) {
                    theResult.push_back("");
                }
            }
        }
        if (theResult.back().size() == 0) {
            theResult.pop_back();
        }
        return theResult;
    }

    int
    getEnumFromString(const std::string & theString, const char * const theStringList[]) {
        int myCounter = -1;
        while(theStringList[++myCounter]) {
            if (theString == theStringList[myCounter]) {
                return myCounter;
            }
        }

        throw ParseException(std::string("Could not find enum for: ") + theString, "getEnumFromString()");
    }

    void
    findAndReplace(std::string & theString, const char * theSearchString, const char * theReplaceString)
    {
        long l1 = strlen(theSearchString);
        long l2 = strlen(theReplaceString);
        if(l1 == 0)    // nothing to replace
            return;
        std::string::size_type idx = -l2;
        while((idx = theString.find(theSearchString, idx+l2)) != std::string::npos)
            theString.replace(idx, l1, theReplaceString);
    }

    std::string
    foundAndReplaced(const std::string & theSource, const std::string & theSearchString, const std::string & theReplaceString) {
        if (theSearchString.empty()) {
            return theSource;
        }
        std::string theResult;
        std::string::size_type theMatchPos = 0;
        std::string::size_type theLastMatchPos = 0;
        while ((theMatchPos = theSource.find(theSearchString, theLastMatchPos)) != string::npos) {
            theResult+=theSource.substr(theLastMatchPos, theMatchPos - theLastMatchPos);
            theResult+=theReplaceString;
            theLastMatchPos = theMatchPos + theSearchString.size();
        }
        if (theLastMatchPos == 0) {
            return theSource;
        } else {
            theResult+=theSource.substr(theLastMatchPos);
        }
        return theResult;
    }

    // Translation Table as described in RFC1113
    const Char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    const Char cb66[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_@=";
    const Char cb67[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789øþƒ";

    // Translation Table to decode (created by author)
    //static const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";
    //static const char cd66[]="|$$$}rstuvwxyz{$$$$$$}>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$|$XYZ[\\]^_`abcdefghijklmnopq";
    static const char   cd67[]="|$$$}rstuvwxyz{$$$$$$}>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$|$XYZ[\\]^_`abcdefghijklmnopq$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$|$$$$$}";

    // encode 3 8-bit binary bytes as 4 '6-bit' characters
    static void encodeBase64(const unsigned char in[3], unsigned char out[4], int len, const Char * cb) {
        out[0] = cb[ in[0] >> 2 ];
        if (len == 1) {
            out[1] = cb[ ((in[0] & 0x03) << 4) ];
            out[2] = cb[64];
            out[3] = cb[64];
        } else if (len == 2) {
            out[1] = cb[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
            out[2] = cb[ ((in[1] & 0x0f) << 2) ];
            out[3] = cb[64];
        } else if (len == 3) {
            out[1] = cb[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
            out[2] = cb[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ];
            out[3] = cb[ in[2] & 0x3f ];
        }
    }

    // decode 4 '6-bit' characters into 3 8-bit binary bytes
    static void decodeBase64(const unsigned char in[4], unsigned char out[3], unsigned int maxLength) {
        // attention: unusual switch construct
        switch (maxLength) {
            default:
            case 3:
                out[2] = (unsigned char) (((in[2] << 6) & 0xc0) | in[3]);
            case 2:
                out[1] = (unsigned char) (in[1] << 4 | in[2] >> 2);
            case 1:
                out[0] = (unsigned char) (in[0] << 2 | in[1] >> 4);
                break;
            case 0:
                throw BufferTooSmall("zero sized buffer passed as argument", PLUS_FILE_LINE);
        }
    }

    void
    binToBase64(const unsigned char * theData, unsigned int theLength, string & theDest, const Char * theCodeTable) {
        unsigned myFullSourceChunks = theLength / 3;
        unsigned myRestSourceChunkSize = theLength % 3;
        unsigned myRestDestChunkSize = myRestSourceChunkSize ? 4 : 0;
        unsigned myResultSize = myFullSourceChunks * 4 + myRestDestChunkSize;
        theDest.resize(myResultSize);
        unsigned i = 0;

        for (; i < myFullSourceChunks; ++i) {
            encodeBase64(theData + i * 3, (unsigned char *)(&theDest[i * 4]), 3, theCodeTable);
        }
        if (myRestSourceChunkSize) {
            encodeBase64(theData + i * 3, (unsigned char *)&theDest[i * 4], myRestSourceChunkSize, theCodeTable);
        }
    }

    unsigned
    base64ToBin(const std::string & theSource, unsigned char * theDest, unsigned int maxLength) {
        if (theSource.empty()) {
            return 0;
        }
        if ((theSource.size() % 4) != 0) {
            throw ParseException("Base64 string length must be a muliple of four", PLUS_FILE_LINE);
        }
        unsigned mySourceChunks    = unsigned(theSource.size() / 4);
        unsigned myDestinationSize = mySourceChunks * 3;
        if (theSource[theSource.size() - 1] == '=' || theSource[theSource.size() - 1] == 'ƒ') {
            myDestinationSize--;
        }
        if (theSource[theSource.size() - 2] == '=' || theSource[theSource.size() - 2] == 'ƒ') {
            myDestinationSize--;
        }
        if (maxLength < myDestinationSize) {
            throw BufferTooSmall(std::string("The provided buffer of size ") + as_string(maxLength) +
                " is too small. Required size: " + as_string(myDestinationSize), PLUS_FILE_LINE);
        }
        for (unsigned i = 0; i < mySourceChunks; ++i) {
            unsigned char in[4];
            for (unsigned j = 0; j < 4; ++j) {
                unsigned myPosition = i * 4 + j;
                unsigned char myChar = theSource[myPosition];
                if (myChar == '=' || myChar == 131 /*'ƒ'*/) {
                    in[j] = 0;
                } else {
                    if (myChar < 43 || myChar > 254 || cd67[myChar - 43] == '$') {
                        throw ParseException(std::string("Invalid character in base64 stream: '") + (char)myChar + "(" + as_string((int)myChar) + ")' at pos: " + as_string(myPosition),                                                    PLUS_FILE_LINE);
                    }

                    in[j] = cd67[myChar - 43] - 62;
                }
            }
            decodeBase64(in, theDest, maxLength);
            theDest += 3;
            maxLength -=3;
        }

        return myDestinationSize;
    }

    std::string getNextLine(int& theCurPos, const string& theString) {
        std::string::size_type myCRPos = theString.find("\n", theCurPos);
        if (myCRPos == string::npos) {
            return "";
        } else {
            string myNextLine = theString.substr(theCurPos, myCRPos-theCurPos);
            theCurPos = myCRPos+1;
            return myNextLine;
        }
    }
#ifdef _WIN32
    char* convertBSTRToLPSTR(BSTR theBSTR) {
        LPSTR pszOut = NULL;
        if (theBSTR != NULL) {
            int nInputStrLen = SysStringLen(theBSTR);

            // Double NULL Termination
            int nOutputStrLen = WideCharToMultiByte(CP_ACP, 0, theBSTR, nInputStrLen, NULL, 0, 0, 0) + 2;
            pszOut = new char [nOutputStrLen];

            if (pszOut) {
                memset(pszOut, 0x00, sizeof (char)*nOutputStrLen);
                WideCharToMultiByte(CP_ACP, 0, theBSTR, nInputStrLen, pszOut, nOutputStrLen, 0, 0);
            }
        }
        return pszOut;
    }

    char* convertLPWSTRToLPSTR(LPWSTR theLPWSTR) {
        LPSTR pszOut = NULL;
        if (theLPWSTR != NULL) {
            int nInputStrLen = wcslen(theLPWSTR);

            // Double NULL Termination
            int nOutputStrLen = WideCharToMultiByte(CP_ACP, 0, theLPWSTR, nInputStrLen, NULL, 0, 0, 0) + 2;
            pszOut = new char [nOutputStrLen];

            if (pszOut) {
                memset(pszOut, 0x00, nOutputStrLen);
                WideCharToMultiByte(CP_ACP, 0, theLPWSTR, nInputStrLen, pszOut, nOutputStrLen, 0, 0);
            }
        }
        return pszOut;
    }
#endif

}

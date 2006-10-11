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
//    $RCSfile: string_functions.cpp,v $
//
//   $Revision: 1.9 $
//
// Description: string helper functions
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "string_functions.h"

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
        static char * hexString = "0123456789ABCDEF";
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
            theData[i] = upperNibble << 4 | lowerNibble;
        }
        return true;
    }

    std::vector<std::string> splitString(const std::string & theString, const std::string & theSeparators) {
        std::vector<std::string> theResult;
        theResult.push_back("");
        for (int i = 0; i < theString.size();++i) {
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
    getEnumFromString(const std::string & theString, const char * theStringList[]) {
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
        long theMatchPos = 0;
        long theLastMatchPos = 0;
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
    const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    const char cb66[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_@=";
    const char cb67[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789øþƒ";

    // Translation Table to decode (created by author)
    //static const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";
    //static const char cd66[]="|$$$}rstuvwxyz{$$$$$$}>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$|$XYZ[\\]^_`abcdefghijklmnopq";
    static const char   cd67[]="|$$$}rstuvwxyz{$$$$$$}>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$|$XYZ[\\]^_`abcdefghijklmnopq$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$|$$$$$}";

    // encode 3 8-bit binary bytes as 4 '6-bit' characters
    static void encodeBase64(const unsigned char in[3], unsigned char out[4], int len, const char * cb) {
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
    binToBase64(const unsigned char * theData, unsigned int theLength, string & theDest, const char * theCodeTable) {
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
}

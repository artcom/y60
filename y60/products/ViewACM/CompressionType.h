//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.

#if !defined(INCL_TEXTURE_COMPRESSION_TYPE)
#define INCL_TEXTURE_COMPRESSION_TYPE
#include <string>
#include <asl/base/string_functions.h>
#include <asl/base/MappedBlock.h>
#include <stdio.h>

namespace  TextureCompression
{

struct OGLT_Header {
    char          MagicNumber[4];
    long          Version;
    long          HeaderSize;
// image storage
    unsigned long Width;    // Reader (Movie), Decompressor(MovieScreen) 
    unsigned long Height;  // Reader (Movie), Decompressor(MovieScreen)
// image presentation
    int           WidthAspect; // MovieScreen(Region)
    int           HeightAspect; // MovieScreen(Region) 
    int           Orientation; // MovieScreen(Region)
// image storage
    unsigned long Components; //Decompressor(MovieScreen) 
    long          OpenGLCompression; //Decompressor(MovieScreen) 
// movie Presentation
    float         FramesPerSecond; // Reader, Player
// image storage
    unsigned long FrameDataSize; // Reader, Decompressor(MovieScreen)
// movie storage
    unsigned long FrameCount; // Reader
};

enum COMPRESSION_FORMAT {
    GENERIC_RGB,
    GENERIC_RGBA,
    GENERIC_ALPHA,
    GENERIC_LUMINANCE,
    GENERIC_LUMINANCE_ALPHA,
    GENERIC_INTENSITY,
    S3TC_DXT1,
    S3TC_DXT1A,
    S3TC_DXT3,
    S3TC_DXT5,
    INVALID_COMPRESSION_FORMAT
};

bool writeOGLTHeader(asl::WriteableFile & theBlock, const OGLT_Header & myHeader, bool verbose);

bool readOGLTHeader(asl::Ptr<std::ifstream> & theBlock, OGLT_Header & header, bool verbose);
bool readOGLTHeader(asl::Ptr<asl::ReadableFile> & theBlock, OGLT_Header & header, bool verbose);
void printOGLTHeader(const OGLT_Header & myHeader);
bool compatibleFormat(const OGLT_Header & firstHeader, const OGLT_Header & secondHeader);

// get static stringlist of compression formats, can be used to translate a string to a 
// enum with getEnum(...), getting a type COMPRESSION_FORMAT or OpenGL type using getGLFormat(...)
// no openGl header included here
const char** getCompressionFormatSL();
int getGLFormat(COMPRESSION_FORMAT);
COMPRESSION_FORMAT getFormatFromGLFormat(int myGLFormat);
int getCompressionRatio(COMPRESSION_FORMAT); 

template<class ENUM>
bool getEnum(const char** stringList, const char* str, ENUM& enumParam) {
    int counter = -1;
    while(stringList[++counter] != 0)
    {
        if (strcmp(stringList[counter], str)==0) {enumParam = (ENUM)counter;return true;}
    }
    return false;
}


template<class ENUM>
std::string getStringFromEnum(const char** stringList, const ENUM& enumParam) {
    int counter = -1;
    while(stringList[++counter] != 0) {}
    if ((int)enumParam<=counter)
        return std::string(stringList[(int)enumParam]);
    else
        return std::string(asl::as_string(enumParam));
}

}
#endif

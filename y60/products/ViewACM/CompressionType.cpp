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

#include "CompressionType.h"

#ifndef WIN32
#define  GL_GLEXT_PROTOTYPES
#else
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glext.h>

#include <string.h>
#include <errno.h>
#include <assert.h>

//#include <libgen.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <unistd.h>
//#include <fcntl.h>


using namespace std;
using namespace asl;




namespace  TextureCompression
{
    const char** getCompressionFormatSL() {
        static const char* COMPRESSION_FORMAT_SL[] = 
        {
            "GENERIC_RGB","GENERIC_RGBA",
            "GENERIC_ALPHA","GENERIC_LUMINANCE",
            "GENERIC_LUMINANCE_ALPHA",
            "GENERIC_INTENSITY","S3TC_DXT1",
            "S3TC_DXT1A","S3TC_DXT3","S3TC_DXT5",0
        };
        return &COMPRESSION_FORMAT_SL[0];
    }   

    
    int getGLFormat(COMPRESSION_FORMAT myFormat) {
        int openGlCompression = -1;
#ifdef GL_ARB_texture_compression    
        switch (myFormat) {
            case GENERIC_RGB:
                openGlCompression = GL_COMPRESSED_RGB_ARB;
                break;
            case GENERIC_RGBA:
                openGlCompression = GL_COMPRESSED_RGBA_ARB;
                break;
            case GENERIC_ALPHA:
                openGlCompression = GL_COMPRESSED_ALPHA_ARB;
                break;
            case GENERIC_LUMINANCE:
                openGlCompression = GL_COMPRESSED_LUMINANCE_ARB;
                break;
            case GENERIC_LUMINANCE_ALPHA:
                openGlCompression = GL_COMPRESSED_LUMINANCE_ALPHA_ARB;
                break;
            case GENERIC_INTENSITY:
                openGlCompression = GL_COMPRESSED_INTENSITY_ARB;
                break;
            case S3TC_DXT1:
                openGlCompression = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
                break;
            case S3TC_DXT1A:
                openGlCompression = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
                break;
            case S3TC_DXT3:
                openGlCompression = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
                break;
            case S3TC_DXT5:
                openGlCompression = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
                break;
            default:
                cerr << "Invalid COMPRESSION_FORMAT " << myFormat << endl;
                assert(0);
        }
#endif
        return openGlCompression;
    }

    COMPRESSION_FORMAT getFormatFromGLFormat(int myGLFormat) {
        COMPRESSION_FORMAT compression = INVALID_COMPRESSION_FORMAT;
#ifdef GL_ARB_texture_compression    
        switch (myGLFormat) {
            case GL_COMPRESSED_RGB_ARB:
                compression = GENERIC_RGB;
                break;
            case GL_COMPRESSED_RGBA_ARB:
                compression = GENERIC_RGBA;
                break;
            case GL_COMPRESSED_ALPHA_ARB:
                compression = GENERIC_ALPHA;
                break;
            case GL_COMPRESSED_LUMINANCE_ARB:
                compression = GENERIC_LUMINANCE;
                break;
            case GL_COMPRESSED_LUMINANCE_ALPHA_ARB:
                compression = GENERIC_LUMINANCE_ALPHA;
                break;
            case GL_COMPRESSED_INTENSITY_ARB:
                compression = GENERIC_INTENSITY;
                break;
            case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
                compression = S3TC_DXT1;
                break;
            case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
                compression = S3TC_DXT1A;
                break;
            case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
                compression = S3TC_DXT3;
                break;
            case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
                compression = S3TC_DXT5;
                break;
            default:
                cerr << "Invalid GL COMPRESSION_FORMAT " << myGLFormat << endl;
                assert(0);
        }
#endif
        return compression;
    }
    
    int getCompressionRatio(COMPRESSION_FORMAT myFormat) {
        int theCompressionRatio = -1;
#ifdef GL_ARB_texture_compression    
        switch (myFormat) {
            case GENERIC_RGB:
            case GENERIC_RGBA:
            case GENERIC_ALPHA:
            case GENERIC_LUMINANCE:
            case GENERIC_LUMINANCE_ALPHA:
            case GENERIC_INTENSITY:
                theCompressionRatio = 0;
                break;
            case S3TC_DXT1:
            case S3TC_DXT1A:
                theCompressionRatio = 8;
                break;
            case S3TC_DXT3:
            case S3TC_DXT5:
                theCompressionRatio = 4;
                break;
            default:
                cerr << "Invalid COMPRESSION_FORMAT " << myFormat << endl;
                assert(0);
        }
#endif
        return theCompressionRatio;
    }

    void printOGLTHeader(const OGLT_Header & header) {
        cout << "OGLT_Header     : " << endl;
        cout << "MagicNumber     : " << header.MagicNumber << endl;
        cout << "Version         : " << header.Version << endl;
        cout << "HeaderSize      : " << header.HeaderSize << endl;
        cout << "Width           : " << header.Width << endl;
        cout << "Height          : " << header.Height << endl;
        cout << "WidthAspect     : " << header.WidthAspect << endl;
        cout << "HeightAspect    : " << header.HeightAspect << endl;
        cout << "Orientation     : " << header.Orientation << endl;
        cout << "Components      : " << header.Components << endl;
        cout << "Format          : " << header.OpenGLCompression << endl;
        cout << "FramesPerSecond : " << header.FramesPerSecond << endl;
        cout << "FrameDataSize   : " << header.FrameDataSize << endl;
        cout << "FrameCount      : " << header.FrameCount << endl << endl;
    }

    bool readOGLTHeader(asl::Ptr<asl::ReadableFile> & theBlock, OGLT_Header & header, bool verbose) {
        theBlock->readBytes(&header, sizeof(header), 0);
        if (strcmp(header.MagicNumber, "a+c")==0) {
            if (verbose) {
                cerr << "Read correct header:" << endl;
                printOGLTHeader(header);
            }
            return true;
        }
        cerr << "### ERROR: not an OGLT header (bad magic number)" << endl; 
        return false;
    }
    bool readOGLTHeader(asl::Ptr<std::ifstream> & theBlock, OGLT_Header & header, bool verbose) {
        theBlock->seekg(0);
        theBlock->read((char*)&header, sizeof(header));
        if (strcmp(header.MagicNumber, "a+c")==0) {
            if (verbose) {
                cerr << "Read correct header:" << endl;
                printOGLTHeader(header);
            }
            return true;
        }
        cerr << "### ERROR: not an OGLT header (bad magic number)" << endl; 
        return false;
    }

/*    bool readOGLTHeader(asl::Ptr<ReadableBlockHandle> & theBlock, OGLT_Header & header, bool verbose) {
        theBlock->getBlock().readData(header, 0);
        if (strcmp(header.MagicNumber, "a+c")==0) {
            if (verbose) {
                cerr << "Read correct header:" << endl;
                printOGLTHeader(header);
            }
            return true;
        }
        cerr << "### ERROR: not an OGLT header (bad magic number)" << endl; 
        return false;
    }*/

    bool writeOGLTHeader(asl::WriteableFile & theBlock, const OGLT_Header & myHeader, bool verbose)
    {
       theBlock.appendData(myHeader);
       if (verbose) {
            cerr << "writeOGLTHeader: header written:" << endl;
            printOGLTHeader(myHeader);
       }
       return true;
    }

    bool compatibleFormat(const OGLT_Header & firstHeader, const OGLT_Header & secondHeader) {
        if (memcmp(&firstHeader, &secondHeader, sizeof(OGLT_Header) - 2 * sizeof(unsigned long)) == 0) {
            if (firstHeader.FrameDataSize == 0 || secondHeader.FrameDataSize == 0 ||
                    firstHeader.FrameDataSize ==  secondHeader.FrameDataSize) {
                return true;
            }
        }
        return false;
    }
}


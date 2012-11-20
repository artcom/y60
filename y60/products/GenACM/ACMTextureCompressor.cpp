//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or dupli:mcated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: TextureCompressor.cpp,v $
//
//     $Author: valentin $
//
//   $Revision: 1.2 $
//
//
// Description: 
//
// (CVS log at the bottom of this file)
//
//
//=============================================================================


//#include <asl/settings.h>
#include <asl/base/file_functions.h>
#include <asl/base/MappedBlock.h>

#ifdef _SETTING_DISABLE_LONG_DEBUG_SYMBOL_WARNING_
#pragma warning(disable:4786)  // Debug symbols too long for std::map :-(
#endif

#include <stdexcept>
#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <asl/math/numeric_functions.h>
#include <paintlib/Filter/plfiltercrop.h>
#include <paintlib/Filter/plfilterflip.h>
#include <paintlib/Filter/plfilterrotate.h>
#include <paintlib/Filter/plfilterresizebilinear.h>

#include "ACMTextureCompressor.h"

namespace TextureCompression {

using namespace std;  // automatically added!
using namespace asl;

TextureCompressor::TextureCompressor (COMPRESSION_FORMAT myFormat, bool verbose)  
    : _myFormat(myFormat), _myVerbose(verbose) 
{
}

TextureCompressor::~TextureCompressor() {}

void
TextureCompressor::fillOGLTHeader(int width, int height, 
        OGLT_Header & myHeader,
        int compressedDataSize,
        long theCompressionFormat,
        int theWidthAspect,
        int theHeightAspect,
        int theOrientation,
        float theFramesPerSecond)
{
    strcpy (myHeader.MagicNumber, "a+c");
    myHeader.Version = 3060;
    myHeader.Width = width;
    myHeader.Height = height;
    myHeader.HeaderSize = sizeof(OGLT_Header);
    myHeader.Components = 4;
    myHeader.OpenGLCompression = theCompressionFormat;
    myHeader.WidthAspect = theWidthAspect;
    myHeader.HeightAspect = theHeightAspect;
    myHeader.Orientation = theOrientation;
    myHeader.FramesPerSecond = theFramesPerSecond;
    myHeader.FrameDataSize = compressedDataSize;
}


int 
TextureCompressor::compress(PLAnyBmp& AnyBmp, GLubyte* & img) 
{

    int compressedSize = 0;
    int format = getGLFormat(_myFormat);                      
    if (format > 0) {
        int myPixelFormat;

        if (AnyBmp.HasAlpha()) {
            myPixelFormat = GL_RGBA;
        } else {
            myPixelFormat = GL_RGB;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, 
                AnyBmp.GetWidth(), AnyBmp.GetHeight(),
                0, myPixelFormat, GL_UNSIGNED_BYTE, (GLubyte *)(AnyBmp.GetPixels()));

        int isCompressed = 0;
#ifdef GL_ARB_texture_compression
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_ARB, &isCompressed);
        if (isCompressed == GL_TRUE) {
            if (_myVerbose)
                cerr << "Retrieving if compressed.." << endl;
#ifdef GL_TEXTURE_IMAGE_SIZE_ARB
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_IMAGE_SIZE_ARB,
                    &compressedSize);
            if (_myVerbose)
                cerr << "Retrieved size..(1)" << endl;
#endif
#ifdef GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB,
                    &compressedSize);
            if (_myVerbose)
                cerr << "Retrieved size..(2)" << endl;
#endif
            img = new GLubyte[compressedSize];
            glGetCompressedTexImageARB(GL_TEXTURE_2D, 0, img);
        }

    }
#endif
    return compressedSize;
}

void
TextureCompressor::storeFrame(asl::WriteableFile & theBlock,
        GLubyte* img,
        const std::string & theFileName,
        int height, int width, 
        long theIndex, 
        int theWidthAspect, 
        int theHeightAspect, 
        int theOrientation, 
        float theFramesPerSecond,
        int compressedSize,
        int theFramecount,
         bool theCreateHeaderFlag) 
{
    if (_myVerbose)
        cerr << "File succesfully openend.." << endl;             

    if (theCreateHeaderFlag) {
        OGLT_Header newHeader;
        fillOGLTHeader(width, height, newHeader,
                0, // compressed data size
                _myFormat, 
                theWidthAspect,
                theHeightAspect,
                theOrientation,
                theFramesPerSecond);
        newHeader.FrameCount = theFramecount;
        newHeader.FrameDataSize = compressedSize * sizeof(GLubyte); 
        writeOGLTHeader(theBlock, newHeader, _myVerbose);
    }
    long long frameSize = ((long long)compressedSize) * ((long long)sizeof(GLubyte)); 
    theBlock.append(img, frameSize);
}

void    
TextureCompressor::compressAndStoreFrame(asl::WriteableFile & theBlock,
                                         const std::string & theFileName,
                                         PLBmp * pBmp, 
                                         long theIndex, 
                                         int theWidthAspect, 
                                         int theHeightAspect, 
                                         int theOrientation, 
                                         float theFramesPerSecond,
                                         int theFramecount,
                                         bool theCreateHeaderFlag) 
{
    PLAnyBmp myGreatenedBitmap;
    int      myDecreaseData = 0;
    if (!asl::powerOfTwo(pBmp->GetWidth()) && asl::powerOfTwo(pBmp->GetHeight())) {
        // rotate by 90 degree
        pBmp->ApplyFilter (PLFilterFlip());  
        pBmp->ApplyFilter(PLFilterRotate(PLFilterRotate::ninety));  
        if (_myVerbose) {
            cout <<"Nach Rotate : "<<pBmp->GetWidth()<<"x"<<pBmp->GetHeight()<<endl;
        }
        theOrientation = 90;
    }

    int height = pBmp->GetHeight();
    int width  = pBmp->GetWidth();

    if (!asl::powerOfTwo(pBmp->GetHeight())) {
        if (_myVerbose) {
            cout <<"Height is non power of 2!!! : "<<pBmp->GetHeight()<<endl;
        }
        double oldBase =  log (float(pBmp->GetHeight()))/log(float(2));
        if (_myVerbose) {
            cout <<"oldBase :"<<oldBase<<endl; 
        }
        double newBase = ceil(oldBase);
        if (_myVerbose) {
            cout <<"newBase :"<<newBase<<endl; 
        }
        int newSize = int(pow(2.0, newBase));
        if (_myVerbose) {
            cout <<"New size: "<<newSize<<endl;
        }
        myGreatenedBitmap.Create(pBmp->GetWidth(), newSize, PLPixelFormat::A8R8G8B8);
        for (int y=0; y<pBmp->GetHeight(); y++) {
            for (int x=0; x<pBmp->GetWidth(); x++) {
                PLPixel32 SrcPixel = pBmp->GetPixel32(x,y);
                myGreatenedBitmap.SetPixel ( x, y, 
                        PLPixel32 ( SrcPixel.GetR(), 
                            SrcPixel.GetG(), 
                            SrcPixel.GetB(), 
                            SrcPixel.GetA()
                            )
                        );
            }
        }
        //
        myDecreaseData = ( ( myGreatenedBitmap.GetHeight() * myGreatenedBitmap.GetWidth() ) -  
                ( pBmp->GetHeight() * pBmp->GetWidth() ) ) 
            * 4 / getCompressionRatio(_myFormat); 
        if (_myVerbose) {
            cout <<"big   bitmap size: "<<( myGreatenedBitmap.GetHeight() * myGreatenedBitmap.GetWidth() ) 
                *4 / getCompressionRatio(_myFormat) <<endl; 
            cout <<"small bitmap size: "<<( pBmp->GetHeight() * pBmp->GetWidth() )*4 / getCompressionRatio(_myFormat) <<endl; 
            cout <<"decreased by:      "<<myDecreaseData<<endl;
        }
        pBmp = &myGreatenedBitmap;

    }



    PLAnyBmp AnyBmp;
#ifdef WIN32
    // RGB are in the wrong order in windows :-(.
    AnyBmp.Create (pBmp->GetWidth(), pBmp->GetHeight(), PLPixelFormat::A8R8G8B8);
    for (int y=0; y<pBmp->GetHeight(); y++) {
        for (int x=0; x<pBmp->GetWidth(); x++) {
            PLPixel32 SrcPixel = pBmp->GetPixel32(x,y);
            AnyBmp.SetPixel ( x, y, 
                    PLPixel32 ( SrcPixel.GetB(), 
                        SrcPixel.GetG(), 
                        SrcPixel.GetR(), 
                        SrcPixel.GetA()
                        )
                    );
        }
    }
#else
    AnyBmp = *pBmp;
#endif
    GLubyte * img = 0;
    int compressedSize = compress(AnyBmp, img);
    storeFrame(theBlock, 
            img, theFileName,
            height, width, 
            theIndex, 
            theWidthAspect,
            theHeightAspect, 
            theOrientation, 
            theFramesPerSecond,
            compressedSize - myDecreaseData,
            theFramecount,
            theCreateHeaderFlag);

    delete [] img;
}
}


//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: GlurFilter.cpp,v 1.5 2004/11/12 15:32:34 janbo Exp $
//   $RCSfile: GlurFilter.cpp,v $
//   $Author: janbo $
//   $Revision: 1.5 $
//   $Date: 2004/11/12 15:32:34 $
//
//
//  Description:
//
//
//=============================================================================

#include "GlurFilter.h"

#include <iostream>

#if defined(_MSC_VER)
#    pragma warning(push,1)
#endif
#include <paintlib/plbitmap.h>
#include <paintlib/planybmp.h>
#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#include <asl/math/numeric_functions.h>

#define DB(x) // x

using namespace std;
using namespace asl;

GlurFilter::GlurFilter(const y60::VectorOfFloat & theParameters) {
    _myColor = Vector4f(
        theParameters[0],
        theParameters[1],
        theParameters[2],
        theParameters[3]
    );
    
    _myColorValue = 
        ((int)(_myColor[0] * 255.0f) << 16) | 
        ((int)(_myColor[1] * 255.0f) << 8) |
        (int)(_myColor[2] * 255.0f);

    _myAlpha = static_cast<unsigned char>(_myColor[3] * 255.0f);
    
    _myRadius = (int)theParameters.size() - 4;
    _myLUT.resize(_myRadius*256);
    //cerr << "Glur radius: " << _myRadius << endl;
    // build the lookup table
    // for each source pixel value and radius combination, we calculate
    // a destination pixel value
    for(int r=0; r<_myRadius; ++r) {
        for(int i=0; i<256; ++i) {
            unsigned char d = static_cast<unsigned char>(theParameters[4 + r] * i);
            if (d>255) {
                d=255;
            }
            _myLUT[r*256+i]=d;
        }
    }
    
    if (_myAlpha > 0) {
        _myAlphaLUT.resize(256*256);
        for (unsigned char i=0; i<256; ++i) {
            for (unsigned char mySourceAlpha=0; mySourceAlpha<256; ++mySourceAlpha) {
                _myAlphaLUT[(mySourceAlpha<<8)+i] = static_cast<unsigned char>((i * _myAlpha * mySourceAlpha) / 255 / 255);
            }
        }    
    }
}

void
GlurFilter::Apply(PLBmpBase * theSource, PLBmp * theDestination) const {
    if ( theSource->GetPixelFormat() != PLPixelFormat::A8R8G8B8 && 
         theSource->GetPixelFormat() != PLPixelFormat::A8B8G8R8 ) {
        // TODO: use right exception class
        throw PLPixelFormat::UnsupportedPixelFormat("GlurFilter (must be A8R8G8B8 or A8B8G8R8)");
    }
    int mySrcWidth = theSource->GetWidth();
    int mySrcHeight = theSource->GetHeight();
    //cerr << "Src: " << mySrcWidth << "x" << mySrcHeight << endl;

    int myDestWidth =  mySrcWidth + 2 * (_myRadius-1);
    int myDestHeight = mySrcHeight + 2 * (_myRadius-1);
    //cerr << "Dest: " << myDestWidth << "x" << myDestHeight << endl;

    PLAnyBmp myTempBmp;
    myTempBmp.Create(myDestWidth, mySrcHeight, theSource->GetPixelFormat(), 0, 0, theSource->GetResolution());

    PLBYTE ** pSrcLineArray = theSource->GetLineArray();
    PLBYTE ** pDstLineArray = myTempBmp.GetLineArray();
    
    int myOffset = _myRadius - 1;
    // pass 1: Glur in x direction
    for (int y=0; y < mySrcHeight; ++y) {
        unsigned * pDest = (unsigned *)pDstLineArray[y];
        unsigned * pSrc = (unsigned *)pSrcLineArray[y];
        for(int x=0; x<myDestWidth; ++x) {
            unsigned int sum = 0;
            for(int r=0; r<_myRadius; ++r) {
                int xs = x + r - myOffset;
                if (xs>=0 && xs<mySrcWidth) {
                    sum += _myLUT[ (r << 8) + ((*(pSrc + xs)) >> 24)];
                }
                if (r!=0) {  // don't take center pixel into account twice!
                    int xs = x - r - myOffset;
                    if (xs>=0 && xs<mySrcWidth) {
                        sum += _myLUT[ (r << 8) + ((*(pSrc + xs)) >> 24)];
                    }
                }
            }
            if (sum>255) {
                sum=255;
            }
            *(pDest + x) = (sum << 24) | 0x00ffffff;
        }
    }

    // Second pass: glur in y direrction
    theDestination->Create(myDestWidth, myDestHeight, theSource->GetPixelFormat(), 0, 0, theSource->GetResolution());
    pSrcLineArray = myTempBmp.GetLineArray();
    pDstLineArray = theDestination->GetLineArray();
    for(int x=0; x<myDestWidth; ++x) {
        for (int y=0; y < myDestHeight; ++y) {
            unsigned * pDest = (unsigned *)pDstLineArray[y];

            unsigned int sum = 0;
            for(int r=0; r<_myRadius; ++r) {
                int ys = y + r - myOffset;
                if (ys>=0 && ys<mySrcHeight) {
                    unsigned * pSrc = (unsigned *)pSrcLineArray[ys];
                    sum += _myLUT[ (r << 8) + ((*(pSrc + x)) >> 24)];
                }
                if (r!=0) {  // don't take center pixel into account twice!
                    int ys = y - r - myOffset;
                    if (ys>=0 && ys<mySrcHeight) {
                        unsigned * pSrc = (unsigned *)pSrcLineArray[ys];
                        sum += _myLUT[ (r << 8) + ((*(pSrc + x)) >> 24)];
                    }
                }
            }
            if (sum>255) {
                sum=255;
            }
            *(pDest + x) = (sum << 24) | _myColorValue;
        }
    }
/*
    // third pass, if alpha !=0, blend original image on top of glurred alpha channel
    if (_myAlpha > 0) {  
        PLBYTE ** pSrcLineArray = theSource->GetLineArray();
        PLBYTE ** pDstLineArray = theDestination->GetLineArray();
        
        int myGlurRed = (int)(_myColor[0] * 255.0f);
        int myGlurGreen = (int)(_myColor[1] * 255.0f);
        int myGlurBlue = (int)(_myColor[2] * 255.0f);
        
        for (int y=0; y < mySrcHeight; ++y) {
            unsigned * pSrc = (unsigned *)pSrcLineArray[y];        
            unsigned * pDest = ((unsigned *)pDstLineArray[y+myOffset]) + myOffset;
            for(int x=0; x<mySrcWidth; ++x) {
                unsigned int mySourceAlphaShifted = (*pSrc & 0xff000000) >> 16;
                unsigned int myDestAlphaShifted = 0x0000ff00 - mySourceAlphaShifted;
               
                unsigned int rs = _myAlphaLUT[(*pSrc & 0xffff0000) >> 16]
                                + _myAlphaLUT[myGlurRed | myDestAlphaShifted];
                
                unsigned int gs = _myAlphaLUT[((*pSrc & 0x0000ff00)>>8) | mySourceAlphaShifted]
                                + _myAlphaLUT[myGlurGreen | myDestAlphaShifted];
                
                unsigned int bs = _myAlphaLUT[(*pSrc & 0x000000ff) | mySourceAlphaShifted]
                                + _myAlphaLUT[myGlurBlue | myDestAlphaShifted];
                                
                *pDest++ = (*pDest & 0xff000000) | (rs << 16) | (gs << 8)| bs;
                pSrc++;
            }
        }   
    }
    */
}

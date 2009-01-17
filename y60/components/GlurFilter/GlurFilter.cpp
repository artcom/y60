/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below. 
//    
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
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
            unsigned int d = theParameters[4 + r] * i;
            if (d>255) {
                d=255;
            }
            _myLUT[r*256+i] = static_cast<unsigned char>(d);
        }
    }
    
    if (_myAlpha > 0) {
        _myAlphaLUT.resize(256*256);
        for (unsigned int i=0; i<256; ++i) {
            for (unsigned int mySourceAlpha=0; mySourceAlpha<256; ++mySourceAlpha) {
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

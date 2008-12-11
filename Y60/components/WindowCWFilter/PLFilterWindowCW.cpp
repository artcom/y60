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
//   $Id: PLFilterWindowCW.cpp,v 1.6 2005/01/26 14:32:43 david Exp $
//   $RCSfile: PLFilterWindowCW.cpp,v $
//   $Author: david $
//   $Revision: 1.6 $
//   $Date: 2005/01/26 14:32:43 $
//
//
//  Description:
//
//
//=============================================================================

#include <iostream>
#include <sstream>
#include <vector>

#if defined(_MSC_VER)
#    pragma warning(push,1)
#endif
#include <paintlib/plbitmap.h>
#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#include <asl/math/numeric_functions.h>

#include "PLFilterWindowCW.h"

#define DB(x) // x

using namespace std;
using namespace asl;

PLFilterWindowCW :: PLFilterWindowCW(float theWindowCenter, float theWindowWidth) :
    _myCenter(theWindowCenter), _myWidth(theWindowWidth) 
{ 
    DB(cerr << "PLFilterWindowCW :: PLFilterWindowCW(" << theWindowCenter << "," << theWindowWidth << ")" << endl);
}

template<int WORDSIZE>  // 16 > WRODSIZE > 8
unsigned int getWordAtIndex(void * theData, int theIndex) {    
    int myFirstByte = (theIndex*WORDSIZE) / 8;    
    int myFirstByteBitOffset = (theIndex*WORDSIZE) % 8;
    
    unsigned char * myData = (unsigned char *)theData;
    // this assumes most significant bit first and most significant byte first
    return (myData[myFirstByte] << myFirstByteBitOffset) | (myData[myFirstByte+1] >> (8 - myFirstByteBitOffset));
}

void
PLFilterWindowCW::Apply(PLBmpBase * theSource, PLBmp * theDestination) const {
    DB(cerr << "windowcw:: apply" << endl);
    int mySrcBPP = theSource->GetBitsPerPixel();
    DB(cerr << "bpp=" << mySrcBPP << endl);
    if ( mySrcBPP != 16 && mySrcBPP != 12 && mySrcBPP != 10 && mySrcBPP != 8) {      
        ostringstream ss;
        ss << "PLFilterWindowCW supports 8, 10, 12 and 16 bit sources only.";
        ss << "(" << mySrcBPP << "bit given)";
        throw UnsupportedPixelFormat(ss.str(), PLUS_FILE_LINE);
    }

    int myLUTSize = theSource->GetNumColors();
    DB(cerr << "myLUTSize=" << myLUTSize << endl);
	vector<unsigned char> myLUT(myLUTSize);

    // you might want to optimize the following
    float m = 1.0f / _myWidth;
    float d = 0.5f - m * _myCenter;
    for (int i=0; i<myLUTSize; ++i) {
        float x = (float)i / (float)myLUTSize;        
        float y = m * x + d;
        int o = (int)(y * 256.0f);
        if (o < 0) {
            o = 0;
        }
        if (o > 255) {
            o = 255;
        }
        myLUT[i] = (unsigned char)(o);
    }

    theDestination->Create(theSource->GetWidth(), theSource->GetHeight(), PLPixelFormat::L8, 0, 0, theSource->GetResolution());
    
    PLBYTE ** pSrcLineArray = theSource->GetLineArray();
    PLBYTE ** pDstLineArray = theDestination->GetLineArray();

    unsigned myBytesPerPixel    = theSource->GetBitsPerPixel() / 8;
    (void)myBytesPerPixel;
    DB(cerr << "myBytesPerPixel=" << myBytesPerPixel << endl);
    //unsigned mySrcBytesPerLine  = myBytesPerPixel * theSource->GetWidth();
    //unsigned myDestBytesPerLine = 1  * theSource->GetWidth();
    unsigned mySourceHeight     = theSource->GetHeight();
    unsigned mySourceWidth		= theSource->GetWidth();
    if (mySrcBPP == 8) {

        for (unsigned y = 0; y < mySourceHeight; ++y) {
            unsigned char * pSrcLine = pSrcLineArray[y];
            unsigned char* pDstLine = pDstLineArray[y];            
            for (unsigned i = 0; i < mySourceWidth; ++i) {
                *pDstLine++ = myLUT[*pSrcLine++];
            }
        }
    } else if (mySrcBPP == 16) {
        for (unsigned y = 0; y < mySourceHeight; ++y) {
            unsigned short * pSrcLine = (unsigned short*)pSrcLineArray[y];
            unsigned char * pDstLine = pDstLineArray[y];            
            for (unsigned i = 0; i < mySourceWidth; ++i) {
                *pDstLine++ = myLUT[*pSrcLine++];
            }
        }
    } else if (mySrcBPP == 10) {
        for (unsigned y = 0; y < mySourceHeight; ++y) {
            unsigned char * pDstLine = pDstLineArray[y];            
            for (unsigned i = 0; i < mySourceWidth; ++i) {        
                *pDstLine++ = myLUT[getWordAtIndex<10>(pSrcLineArray[y], i)];
            }
        }
    } else if (mySrcBPP == 12) {
        for (unsigned y = 0; y < mySourceHeight; ++y) {
            unsigned char * pDstLine = pDstLineArray[y];            
            for (unsigned i = 0; i < mySourceWidth; ++i) {        
                *pDstLine++ = myLUT[getWordAtIndex<12>(pSrcLineArray[y], i)];
            }
        }
    }
}

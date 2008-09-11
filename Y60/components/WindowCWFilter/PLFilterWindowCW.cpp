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

#include "PLFilterWindowCW.h"

#include <asl/math/numeric_functions.h>

#include <paintlib/plbitmap.h>

#include <iostream>
#include <sstream>
#include <vector>

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
    DB(cerr << "myBytesPerPixel=" << myBytesPerPixel << endl);
	unsigned mySrcBytesPerLine  = myBytesPerPixel * theSource->GetWidth();
    unsigned myDestBytesPerLine = 1  * theSource->GetWidth();
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

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
//   $Id: PLFilterResizePadded.cpp,v 1.1 2005/03/24 23:36:00 christian Exp $
//   $RCSfile: PLFilterResizePadded.cpp,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:36:00 $
//
//
//  Description:
//
//
//=============================================================================

#include "PLFilterResizePadded.h"

#include <asl/numeric_functions.h>
#include <asl/Logger.h>

#include <paintlib/plpixel8.h>
#include <paintlib/plbitmap.h>

#include <iostream>

#define DB(x) // x

using namespace std;
using namespace asl;

void
PLFilterResizePadded::Apply(PLBmpBase * theSource, PLBmp * theDestination) const {
    PLASSERT(_myNewWidth  >= theSource->GetWidth());
    PLASSERT(_myNewHeight >= theSource->GetHeight());

    DB(cerr << "creating dest bitmap (" << _myNewWidth << "x" << _myNewHeight << ")" << endl);
    theDestination->Create(_myNewWidth, _myNewHeight, theSource->GetPixelFormat(), 0, 0, theSource->GetResolution());
    DB(cerr << "created" << endl);

    PLBYTE ** pSrcLineArray = theSource->GetLineArray();
    PLBYTE ** pDstLineArray = theDestination->GetLineArray();

    unsigned myBytesPerPixel    = theSource->GetBitsPerPixel() / 8;
    unsigned mySrcBytesPerLine  = myBytesPerPixel * (theSource->GetWidth() - 1);
    unsigned myDestBytesPerLine = myBytesPerPixel * (_myNewWidth - 1);
    unsigned mySourceHeight     = theSource->GetHeight();
    
    int myRemainingBytes = myDestBytesPerLine - mySrcBytesPerLine;    
    AC_PRINT << "myRemainingBytes: " << myRemainingBytes << " bytesPerPixel: " << myBytesPerPixel;
    for (unsigned y = 0; y < mySourceHeight; ++y) {
        PLBYTE * pSrcLine = pSrcLineArray[y];
        PLBYTE * pDstLine = pDstLineArray[y];
        
        // Copy the image itself
        memcpy (pDstLine, pSrcLine, mySrcBytesPerLine);

        // Repeat the last column (to prevent texture repeat artefacts)
        if (myRemainingBytes >= myBytesPerPixel) {
            memcpy(pDstLine + mySrcBytesPerLine, pSrcLine + mySrcBytesPerLine - myBytesPerPixel, myBytesPerPixel);
        }
        
        // Repeat the fist column (to prevent texture repeat artefacts)
        if (myRemainingBytes >= (myBytesPerPixel * 2)) {
            memcpy(pDstLine + myDestBytesPerLine, pSrcLine, myBytesPerPixel);
        }

        // Pad the remaining columns with black pixels (to improve compression ratios)        
        if (myRemainingBytes > (myBytesPerPixel * 2)) {
            memset(pDstLine + mySrcBytesPerLine + myBytesPerPixel, 0, myRemainingBytes - (2 * myBytesPerPixel));
        } 
    }
    
    for (unsigned y = mySourceHeight; y < _myNewHeight; ++y) {        
        PLBYTE * pDstLine = pDstLineArray[y];        
        if (y == mySourceHeight) {
            // Repeat the last line (to prevent texture repeate artefacts)
            memcpy(pDstLine, pDstLineArray[mySourceHeight - 1], myDestBytesPerLine);
        } else if (y == _myNewHeight - 1) {
            // Repeat the first line (to prevent texture repeate artefacts)
            memcpy(pDstLine, pDstLineArray[0], myDestBytesPerLine);        
        } else {        
            // Set the remaining lines to black (to improve compression ratios)
            memset(pDstLine, 0, myDestBytesPerLine);
        }
    } 
}

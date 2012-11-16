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
*/
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

// own header
#include "PLFilterResizePadded.h"

#include <iostream>
#include <cstring>

#include <asl/math/numeric_functions.h>
#include <asl/base/Logger.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plpixel8.h>
#include <paintlib/plbitmap.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#define DB(x) // x

using namespace std;
using namespace asl;

void
PLFilterResizePadded::Apply(PLBmpBase * theSource, PLBmp * theDestination) const {
    PLASSERT(_myNewWidth  >= static_cast<unsigned int>(theSource->GetWidth()));
    PLASSERT(_myNewHeight >= static_cast<unsigned int>(theSource->GetHeight()));

    DB(cerr << "creating dest bitmap (" << _myNewWidth << "x" << _myNewHeight << ")" << endl);
    theDestination->Create(_myNewWidth, _myNewHeight, theSource->GetPixelFormat(), 0, 0, theSource->GetResolution());
    DB(cerr << "created" << endl);


    PLBYTE ** pSrcLineArray = theSource->GetLineArray();
    PLBYTE ** pDstLineArray = theDestination->GetLineArray();

    unsigned myBytesPerPixel    = theSource->GetBitsPerPixel() / 8;
    unsigned mySrcBytesPerLine  = myBytesPerPixel * theSource->GetWidth();
    unsigned myDestBytesPerLine = myBytesPerPixel * _myNewWidth;
    unsigned mySourceHeight     = theSource->GetHeight();

    unsigned myRemainingBytes = myDestBytesPerLine - mySrcBytesPerLine;
    //AC_PRINT << "myRemainingBytes: " << myRemainingBytes << " bytesPerPixel: " << myBytesPerPixel;
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
            memcpy(pDstLine + myDestBytesPerLine - myBytesPerPixel, pSrcLine, myBytesPerPixel);
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

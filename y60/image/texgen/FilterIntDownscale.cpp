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
//    $RCSfile: FilterIntDownscale.cpp,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1 $
//
//
// Description:
//
//=============================================================================

#include "FilterIntDownscale.h"

#include <assert.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plpixel32.h>
#include <paintlib/plbitmap.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

namespace TexGen {

FilterIntDownscale::FilterIntDownscale (int myScale) : _myScale (myScale)
{
}

void FilterIntDownscale::Apply (PLBmp* mySourceBmp, PLBmp* myDestBmp) const {
    assert (mySourceBmp->GetBitsPerPixel() == 8);
    PLPoint myDestSize (mySourceBmp->GetWidth()/_myScale,
                        mySourceBmp->GetHeight()/_myScale);
    myDestBmp->Create (myDestSize.x, myDestSize.y, PLPixelFormat::X8R8G8B8);

    int numPixels = _myScale*_myScale;
    PLPixel32 * myPalette = mySourceBmp->GetPalette();
    for (int y=0; y<myDestSize.y; y++) {
        for (int x=0; x<myDestSize.x; x++) {
            int r = 0;
            int g = 0;
            int b = 0;
            for (int mySrcY=y*_myScale; mySrcY<(y+1)*_myScale; mySrcY++) {
                for (int mySrcX=x*_myScale; mySrcX<(x+1)*_myScale; mySrcX++) {
                    PLBYTE myColIndex = mySourceBmp->GetLineArray()[mySrcY][mySrcX];
                    PLPixel32 srcPixel = myPalette[myColIndex];
                    r += srcPixel.GetR();
                    g += srcPixel.GetG();
                    b += srcPixel.GetB();
                }
            }
            myDestBmp->SetPixel( x, y, PLPixel32( static_cast<PLBYTE>(r/numPixels)
                                                , static_cast<PLBYTE>(g/numPixels)
                                                , static_cast<PLBYTE>(b/numPixels) ) );
        }
    }
}


}

//=============================================================================
//
// Copyright (C) 2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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

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

#include "LitTerrainTexGen.h"

#include <assert.h>
#include <stdexcept>
#include <math.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plpixel24.h>
#include <paintlib/plbitmap.h>
#include <paintlib/plpoint.h>
#include <paintlib/plrect.h>
#include <paintlib/planybmp.h>
#include <paintlib/Filter/plfiltercrop.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

namespace TexGen {

    using namespace std;

    void generateMultiplicationMap (const PLBmp & lightMap, PLBmp & destMap) {
        assert(lightMap.GetWidth() == destMap.GetWidth() );
        assert(lightMap.GetHeight() == destMap.GetHeight() );
        assert(lightMap.GetBitsPerPixel() == 8);
        assert(destMap.GetBitsPerPixel() == 8);

        PLBYTE ** myLightMapLines   = lightMap.GetLineArray();
        PLBYTE ** myDestMapLines = destMap.GetLineArray();

        PLBYTE myScale;
        for (int y=0;y<lightMap.GetHeight(); y++) {
            for (int x=0;x<lightMap.GetWidth(); x++) {
                PLBYTE& col = myDestMapLines[y][x];
                myScale =  PLBYTE(myLightMapLines[y][x]);
                int tempInt = (col*myScale)/255;
                col = static_cast<PLBYTE>(min(tempInt, 255));
            }
        }
    }

    void applyMultiplicationMap (PLBmp & textureBmp, const PLBmp & attnBmp,
            const PLRect & srcRect, double myLightingFactor)
    {
        assert(attnBmp.GetBitsPerPixel() == 8);
        assert(textureBmp.GetBitsPerPixel() == 32);
        assert (srcRect.Width() > 0 && srcRect.Height() > 0);

        double Scale = double(srcRect.Width())/textureBmp.GetWidth();
        double YScale = double(srcRect.Height())/textureBmp.GetHeight();
        assert (Scale == YScale); (void)YScale;

        PLBYTE ** myAttnBmpLines   = attnBmp.GetLineArray();
        PLPixel32 ** myTextureBmpLines = (PLPixel32**)textureBmp.GetLineArray();

        if (Scale > 0.99) {
            for (int y=0;y<textureBmp.GetHeight(); y++) {
                for (int x=0;x<textureBmp.GetWidth(); x++) {
                    PLPixel32& col = myTextureBmpLines[y][x];

                    PLBYTE myScale;
                    myScale =  PLBYTE(myAttnBmpLines[int(Scale*y+srcRect.tl.y)]
                                        [int(Scale*x+srcRect.tl.x)]);
                    int tempIntR = int((myLightingFactor * col.GetR() * myScale)/256);
                    int tempIntG = int((myLightingFactor * col.GetG() * myScale)/256);
                    int tempIntB = int((myLightingFactor * col.GetB() * myScale)/256);
                    col.Set( static_cast<PLBYTE>(min(tempIntR,255)),
                             static_cast<PLBYTE>(min(tempIntG,255)),
                             static_cast<PLBYTE>(min(tempIntB,255)), 255 );
                }
            }
        } else {
            int MaxY = attnBmp.GetHeight()-1;
            int MaxX = attnBmp.GetWidth()-1;
            for (int y=0;y<textureBmp.GetHeight(); y++) {
                double ySrcPos = Scale*y+srcRect.tl.y;
                int yLightPos = int (ySrcPos);
                double yLightFade = fmod (ySrcPos, 1);
                if (yLightPos >= MaxY) {
                    yLightPos = MaxY - 1;
                    yLightFade = 1;
                }
                for (int x=0;x<textureBmp.GetWidth(); x++) {
                    PLPixel32& col = myTextureBmpLines[y][x];
                    double xSrcPos = Scale*x+srcRect.tl.x;
                    int xLightPos = int (xSrcPos);
                    double xLightFade = fmod (xSrcPos, 1);
                    if (xLightPos >= MaxX) {
                        xLightPos = MaxX - 1;
                        xLightFade = 1;
                    }

                    double myScale = double(myAttnBmpLines[yLightPos][xLightPos])*
                                      (1-xLightFade)*(1-yLightFade) +
                                  double(myAttnBmpLines[yLightPos][xLightPos+1])*
                                      xLightFade*(1-yLightFade) +
                                  double(myAttnBmpLines[yLightPos+1][xLightPos])*
                                      (1-xLightFade)*yLightFade +
                                  double(myAttnBmpLines[yLightPos+1][xLightPos+1])*
                                      xLightFade*yLightFade;

                    int tempIntR = int((myLightingFactor * col.GetR() * myScale) /256);
                    int tempIntG = int((myLightingFactor * col.GetG() * myScale) /256);
                    int tempIntB = int((myLightingFactor * col.GetB() * myScale) /256);
                    col.Set( static_cast<PLBYTE>(min(tempIntR,255)),
                             static_cast<PLBYTE>(min(tempIntG,255)),
                             static_cast<PLBYTE>(min(tempIntB,255)),
                             255);
                }
            }
        }
    }


}


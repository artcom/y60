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

// own header
#include "HeightToNormalMap.h"

#include <iostream>

#include <asl/math/numeric_functions.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plpixel8.h>
#include <paintlib/plbitmap.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

using namespace std;
using namespace asl;

HeightToNormalMap::HeightToNormalMap() {
}

HeightToNormalMap::~HeightToNormalMap() {
}

void
HeightToNormalMap::Apply(PLBmpBase * theSource, PLBmp * theDestination) const {
  theDestination->Create(theSource->GetWidth(), theSource->GetHeight(), PLPixelFormat::A8R8G8B8);
    //theDestination->SetHasAlpha(true);
    PLPixel8 ** mySourceLines = (PLPixel8**)theSource->GetLineArray();
    PLPixel32 ** myDestinationLines = (PLPixel32**)theDestination->GetLineArray();

    for (int y = 0; y < theSource->GetHeight(); ++y) {
        PLPixel8  * mySourceLine = mySourceLines[y];
        PLPixel32 * myDestinationLine = myDestinationLines[y];

        PLPixel8 * myNextLine;
        if (y + 1 == theSource->GetHeight()) {
            myNextLine = mySourceLines[y];
        } else {
            myNextLine = mySourceLines[y + 1];
        }
        for (int x = 0; x < theSource->GetWidth(); ++x) {
            float myHeight = mySourceLine->Get();
            float aboveNeighbourHeight = myNextLine->Get();
            float rightNeighbourHeight;
            if (x + 1 == theSource->GetWidth()) {
                rightNeighbourHeight = mySourceLine->Get();
            } else {
                rightNeighbourHeight = (mySourceLine + 1)->Get();
            }
            myHeight /= 255;
            aboveNeighbourHeight /= 255;
            rightNeighbourHeight /= 255;
            float myZ = 1.0f / float((sqrt(pow(myHeight - aboveNeighbourHeight, 2) +
                                    pow(myHeight - rightNeighbourHeight, 2) + 1)));
            float myX = (myHeight - rightNeighbourHeight) * myZ;
            float myY = (myHeight - aboveNeighbourHeight) * myZ;

            myDestinationLine->SetR((PLBYTE)minimum(128.0 * myX + 128, 255.0));
            myDestinationLine->SetG((PLBYTE)minimum(128.0 * -myY + 128, 255.0));
            myDestinationLine->SetB((PLBYTE)minimum(128.0 * myZ + 128, 255.0));
            myDestinationLine->SetA(255);

            mySourceLine++;
            myDestinationLine++;
            myNextLine++;
        }
    }
}

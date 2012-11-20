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
//    $RCSfile: TestTextureDefinition.cpp,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1 $
//
//
// Description:
//
//=============================================================================

#include "TestTextureDefinition.h"

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/planybmp.h>
#include <paintlib/plpixel32.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#include "TextureDefinition.h"

using namespace TexGen;
using namespace std;


PLAnyBmp* TestTextureDefinition::createBitmap () {
    PLAnyBmp *theBmp = new PLAnyBmp ();
    theBmp->Create (2,2,PLPixelFormat::X8R8G8B8);
    theBmp->SetPixel (0,0, PLPixel32 (0,0,0,0));
    theBmp->SetPixel (0,1, PLPixel32 (255,0,0,0));
    theBmp->SetPixel (1,0, PLPixel32 (0,255,0,0));
    theBmp->SetPixel (1,1, PLPixel32 (0,0,255,0));

    return theBmp;
}

void TestTextureDefinition::run() {
    TextureDefinition myColorTextureDefinition (0);
    myColorTextureDefinition.addLayer (new LayerDefinition (PLPixel32(255,0,255,0), 1.0));
    myColorTextureDefinition.setTileSize (1);
    ENSURE (myColorTextureDefinition.getPixel(32,15) == PLPixel32(255,0,255,0));

    TextureDefinition myTextureDefinition1 (0);
    myTextureDefinition1.addLayer (new LayerDefinition (createBitmap(), 1, 1, 1.0));
    myTextureDefinition1.setTileSize (4);
    ENSURE (myTextureDefinition1.getPixel(0,0) == PLPixel32 (0,0,0,0));
    ENSURE (myTextureDefinition1.getPixel(3,3) == PLPixel32 (0,0,255,0));
    ENSURE (myTextureDefinition1.getPixel(7,7) == PLPixel32 (0,0,255,0));

    TextureDefinition myTextureDefinition2 (0);
    myTextureDefinition2.addLayer (new LayerDefinition (createBitmap(), 2, 2, 1.0));
    myTextureDefinition2.setTileSize (2);
    ENSURE (myTextureDefinition2.getPixel(0,0) == PLPixel32 (0,0,0,0));
    ENSURE (myTextureDefinition2.getPixel(3,3) == PLPixel32 (0,0,255,0));
    ENSURE (myTextureDefinition2.getPixel(7,7) == PLPixel32 (0,0,255,0));

    myTextureDefinition2.setTileSize (128);
    ENSURE (myTextureDefinition2.getPixel(0,0) == PLPixel32 (0,0,0,0));
    ENSURE (myTextureDefinition2.getPixel(255, 255) == PLPixel32 (0,0,255,0));

    ENSURE (myTextureDefinition2.getAvgColor() == PLPixel32 (63, 63, 63, 0));
}


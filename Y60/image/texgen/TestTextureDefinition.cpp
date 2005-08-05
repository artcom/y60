//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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

#include "TextureDefinition.h"

#include <paintlib/planybmp.h>
#include <paintlib/plpixel32.h>

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


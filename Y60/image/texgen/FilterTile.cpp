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
//    $RCSfile: FilterTile.cpp,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.2 $
//
//
// Description: 
//
//=============================================================================

#include "FilterTile.h"

#include <assert.h>
#include <stdexcept>
#include <sstream>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plbitmap.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

using namespace std;

namespace TexGen {

FilterTile::FilterTile(const vector<PLBmp*> myTileBmps)
    : PLFilter(),
      _myTileBmps (myTileBmps) {
    if (myTileBmps.empty()) {
        throw out_of_range ("No texture bitmaps available.");
    }
    _myTileWidth = myTileBmps[0]->GetWidth();  
    _myTileHeight = myTileBmps [0]->GetHeight();

    testTileSizes();
}

FilterTile::~FilterTile() {
  
}

void FilterTile::Apply(PLBmp *indexBmp, PLBmp * resultBmp) const {
    testTileSizes();

    int resultWidth = _myTileWidth * indexBmp->GetWidth();
    int resultHeight = _myTileHeight * indexBmp->GetHeight();
    resultBmp->Create (resultWidth, resultHeight, PLPixelFormat::X8R8G8B8);
    
    for (int y=0; y<indexBmp->GetHeight(); ++y) {
        for (int x=0; x<indexBmp->GetWidth(); ++x) {
            PLBYTE curTileIndex = (indexBmp->GetLineArray()[y])[x];
            // Check if we have enough texture bitmaps available.
            if (curTileIndex >= _myTileBmps.size()) {
                ostringstream st;
                st << "Pixel (" << x << ", " << y << 
                      ") in index bitmap out of bounds." << ends;
                throw out_of_range (st.str());
            }
            PLBmp * curTileBmp = _myTileBmps[curTileIndex];
            for (int tileY = 0; tileY < _myTileHeight; ++ tileY) {
                PLPixel32* curTileLine = (PLPixel32 *)(curTileBmp->GetLineArray()[tileY]);
                PLPixel32* resultLine = 
                    (PLPixel32 *)(resultBmp->GetLineArray()[y*_myTileHeight+tileY]);
                for (int tileX = 0; tileX < _myTileWidth; ++tileX) {
                    resultLine[x*_myTileWidth+tileX] = curTileLine[tileX];
                }
            }
        }
    }
}

void FilterTile::testTileSizes() const {
    // Check if all the texture bitmaps have the same height and width.
    for (int i=0; i<_myTileBmps.size(); ++i) {
        if (_myTileBmps[i]->GetWidth() != _myTileWidth || 
            _myTileBmps[i]->GetHeight() != _myTileHeight) {
            throw invalid_argument ("Texture sizes not equal.");
        }
    }
}
 
}


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


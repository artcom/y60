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
//    $RCSfile: LayerDefinition.cpp,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1 $
//
//
// Description:
//
//=============================================================================

#include "LayerDefinition.h"

#include <assert.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plbitmap.h>
#include <paintlib/planybmp.h>
#include <paintlib/Filter/pl2passscale.h>
#include <paintlib/Filter/plfilterresizebilinear.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

using namespace std;

namespace TexGen {

LayerDefinition::LayerDefinition (const PLPixel32 myColor, float opacity)
    : _myWidthInTiles (1), _myHeightInTiles (1), _myOpacity(opacity)
{
    _myBmp = new PLAnyBmp;
    _myBmp->Create (1,1,PLPixelFormat::X8R8G8B8);
    ((PLPixel32*)(_myBmp->GetLineArray()[0]))[0] = myColor;
    _myActualBmp = 0;
    _myTileSize = -1;
    calcAvgColor();
}


LayerDefinition::LayerDefinition (PLBmp* myBmp, int myWidthInTiles, int myHeightInTiles, float opacity)
    : _myBmp(myBmp),
      _myWidthInTiles (myWidthInTiles), _myHeightInTiles (myHeightInTiles), _myOpacity(opacity)
{
    PLASSERT_VALID (_myBmp);
    assert (_myBmp->GetBitsPerPixel() == 32);
    _myTileSize = -1;
    _myActualBmp = 0;
    calcAvgColor();
}


LayerDefinition::~LayerDefinition() {
    std::map<int, PLBmp*>::iterator iter;
    for (iter=_myResizedBmpCache.begin(); iter != _myResizedBmpCache.end(); ++iter) {
        delete (iter->second);
    }
    delete _myBmp;
}

void LayerDefinition::setTileSize (double myTileSize) {
   if (_myTileSize != int (myTileSize*256)) {
        assert(_myTileSize!=0);
         _myTileSize = int(myTileSize*256);
        // check if tilesize is already in cache
        assert (_myResizedBmpCache.size() < 20);
        if (_myResizedBmpCache.find(_myTileSize) != _myResizedBmpCache.end()) {
            _myActualBmp = _myResizedBmpCache[_myTileSize];
         }
        else {
            int myBmpWidth = (_myTileSize*_myWidthInTiles)/256;
            int myBmpHeight = (_myTileSize*_myHeightInTiles)/256;
            _myActualBmp  = new PLAnyBmp;
            _myActualBmp->CreateFilteredCopy
                (*_myBmp, PLFilterResizeBilinear (max(1,myBmpWidth), max(1,myBmpHeight)));
            _myResizedBmpCache[int(myTileSize)] = _myActualBmp;
         }
    }
    assert(_myTileSize!=-1);
}

void LayerDefinition::calcAvgColor () {
    int tempRedColor   = 0;
    int tempGreenColor = 0;
    int tempBlueColor  = 0;
    PLPixel32 tempColor;
    for(int y=0; y<_myBmp->GetHeight(); y++) {
      for(int x=0; x<_myBmp->GetWidth(); x++) {
            tempColor = _myBmp->GetPixel32(x,y);
            tempRedColor   += tempColor.GetR();
            tempGreenColor += tempColor.GetG();
            tempBlueColor  += tempColor.GetB();
        }
     }
    int bitmapSize = _myBmp->GetHeight()*_myBmp->GetWidth();
    _myAvgColor.Set ( static_cast<PLBYTE>(tempRedColor/bitmapSize),
                      static_cast<PLBYTE>(tempGreenColor/bitmapSize),
                      static_cast<PLBYTE>(tempBlueColor/bitmapSize), 255 );
    _myAvgColor = _myAvgColor*_myOpacity;

}

}



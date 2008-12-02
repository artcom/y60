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



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
//    $RCSfile: LayerDefinition.h,v $
//
//     $Author: uzadow $
//
// Description:
//
//=============================================================================

#if !defined(INCL_LAYERDEFINITION)
#define INCL_LAYERDEFINITION

#include <assert.h>
#include <map>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plpixel32.h>
#include <paintlib/plbitmap.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

namespace TexGen {

class LayerDefinition {
public:
    LayerDefinition (const PLPixel32 myColor, float opacity);
    LayerDefinition (PLBmp* myBmp, int myWidthInTiles, int myHeightInTiles, float opacity);
    virtual ~LayerDefinition();

    void setTileSize (double myTileSize);
    int getCurSize () const
        { return _myActualBmp->GetWidth(); };
    const PLPixel32 getPixel(int x, int y) const;
    PLPixel32 getAvgColor () const { return _myAvgColor; }
    const PLPixel32* getPixelLine(int x, int y) const;

private:
    LayerDefinition(const LayerDefinition &);
    const LayerDefinition & operator=(const LayerDefinition &);
    void calcAvgColor ();

    PLBmp* _myBmp;
    std::map<int, PLBmp*> _myResizedBmpCache;
    PLBmp* _myActualBmp;
    float  _myOpacity;
    int    _myWidthInTiles;
    int    _myHeightInTiles;
    int    _myTileSize;
    PLPixel32 _myAvgColor;
};

inline const PLPixel32* LayerDefinition::getPixelLine(int x, int y) const {
    assert (_myTileSize != -1);
    PLASSERT_VALID (_myActualBmp);
    return _myActualBmp->GetLineArray32()[y%_myActualBmp->GetHeight()]+(x%_myActualBmp->GetWidth());
}


inline const PLPixel32 LayerDefinition::getPixel(int x, int y) const {
    assert (_myTileSize != -1);
    PLASSERT_VALID (_myActualBmp);
    return _myActualBmp->GetPixel32 (x%_myActualBmp->GetWidth(),
                                    y%_myActualBmp->GetHeight())*_myOpacity;
}

}

#endif



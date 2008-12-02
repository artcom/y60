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



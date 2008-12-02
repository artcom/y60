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
//    $RCSfile: FilterTile.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1 $
//
//
// Description: 
//
//=============================================================================

#if !defined(INCL_FILTERTILE)
#define INCL_FILTERTILE

#include <vector>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/Filter/plfilter.h>
#include <paintlib/plpixel32.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

class PLBmp;

namespace TexGen {

class FilterTile : public PLFilter  
{
public:
    FilterTile (const std::vector<PLBmp*> myTileBmps);
    virtual ~FilterTile();
    virtual void Apply(PLBmp * indexBmp, PLBmp * resultBmp) const;

private:
    void testTileSizes() const;
    std::vector<PLBmp *> _myTileBmps;
    int _myTileWidth;
    int _myTileHeight;
};

}

#endif



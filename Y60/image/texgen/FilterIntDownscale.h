//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: FilterIntDownscale.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1 $
//
//
// Description: 
//
//=============================================================================

#if !defined(INCL_FILTERINTDOWNSCALE)
#define INCL_FILTERINTDOWNSCALE

#include <paintlib/plpixel32.h>
#include <paintlib/plbitmap.h>
#include <paintlib/Filter/plfilter.h>

namespace TexGen {

class FilterIntDownscale: public PLFilter {
public:
    FilterIntDownscale (int myScale);
    virtual void Apply (PLBmp* mySourceBmp, PLBmp* myDestBmp) const;

private:
    int _myScale;
};

}
#endif

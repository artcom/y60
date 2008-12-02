//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: PLFilterResizePadded.h,v 1.1 2005/03/24 23:36:00 christian Exp $
//   $RCSfile: PLFilterResizePadded.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:36:00 $
//
//
//  Description:
//
//
//=============================================================================

#ifndef _ac_y60_PLFilterResizePadded_h_
#define _ac_y60_PLFilterResizePadded_h_

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plpaintlibdefs.h>
#include <paintlib/Filter/plfilter.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

class PLBmp;

class PLFilterResizePadded : public PLFilter {
    public:
        PLFilterResizePadded(unsigned theNewWidth, unsigned theNewHeight) :
            _myNewWidth(theNewWidth), _myNewHeight(theNewHeight) {}
        virtual ~PLFilterResizePadded() {};
        virtual void Apply(PLBmpBase * theSource, PLBmp * theDestination) const;

    private:
        unsigned _myNewWidth;
        unsigned _myNewHeight;
};

#endif

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
//   $Id: PLFilterWindowCW.h,v 1.2 2004/11/09 16:56:34 janbo Exp $
//   $RCSfile: PLFilterWindowCW.h,v $
//   $Author: janbo $
//   $Revision: 1.2 $
//   $Date: 2004/11/09 16:56:34 $
//
//
//  Description:
//
//
//=============================================================================

#ifndef _ac_som_PLFilterWindowCW_h_
#define _ac_som_PLFilterWindowCW_h_

#include <asl/base/Exception.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plpaintlibdefs.h>
#include <paintlib/Filter/plfilter.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

DEFINE_EXCEPTION(UnsupportedPixelFormat, asl::Exception)


class PLBmp;

//  1.0|        ___
//     |       /
//     |      /
//  0.5|---  /
//     |    /
//     |   /   window center
//  0.0|__/  /
//     -----^----- 1.0
//        <---> window width

class PLFilterWindowCW : public PLFilter {
    public:
        PLFilterWindowCW(float theWindowCenter, float theWindowWidth);
        virtual ~PLFilterWindowCW() {};
        virtual void Apply(PLBmpBase * theSource, PLBmp * theDestination) const;

    private:
        float _myCenter;
        float _myWidth;        
};

#endif

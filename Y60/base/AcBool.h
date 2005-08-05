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
//   $Id: AcBool.h,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: AcBool.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//
//  Description: This file contains all the wellknown names for the
//               SOM
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#ifndef _ac_y60_AcBool_h_
#define _ac_y60_AcBool_h_

namespace y60 {

    struct AcBool {
        AcBool() {}
        AcBool(bool b) { _b = b;};
        operator bool() const {
            return _b;
        }
        operator bool&() {
            return _b;
        }
        bool & operator=(bool b) {
            _b = b;
            return *this;
        }
        bool operator==(const bool otherBool) const {
            return ( _b == otherBool );
        }
        bool operator!=(const bool otherBool) const {
            return ( _b != otherBool );
        }
        bool operator==(const AcBool otherBool) const {
            return ( _b == otherBool._b );
        }
        bool operator!=(const AcBool otherBool) const {
            return ( _b != otherBool._b );
        }
        bool _b;
    };
}

namespace asl {
    inline
    y60::AcBool & interpolate(y60::AcBool & a, y60::AcBool & b, const double & theAlpha) {
        if (theAlpha <= 0.5) {
            return a;
        } else {
            return b;
        }
    }
}

#endif


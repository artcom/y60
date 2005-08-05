//=============================================================================
//
// Copyright (C) 2000-2002, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or dupli:mcated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: BandRejectFilter.cpp,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.4 $
//
//
//=============================================================================

#include "BandRejectFilter.h"

#define _USE_MATH_DEFINES 

#include <math.h>



namespace AudioBase {
BandRejectFilter::BandRejectFilter(double theFrquency,
                                   double theBandwidth,
                                   int theSampleRate,
                                   double theVolume) :
    ButterworthFilter("BandRejectFilter", theSampleRate, theVolume),
    _myFrequency(theFrquency),
    _myBandwidth(theBandwidth)
{
    double C = tan(M_PI * _myBandwidth / getSampleRate());
    double D = 2.0 * cos( 2.0 * M_PI * _myFrequency / getSampleRate());

    a[0] = 1.0 / (1.0 +C);
    a[1] = -D * a[0];
    a[2] = a[0];

    b[0] = a[1];
    b[1] = (1.0 - C) * a[0];
}

}

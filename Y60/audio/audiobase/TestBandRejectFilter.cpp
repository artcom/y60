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
//    $RCSfile: TestBandRejectFilter.cpp,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.2 $
//
//=============================================================================

#include "TestBandRejectFilter.h"
#include "BandRejectFilter.h"

using namespace AudioBase;

void 
TestBandRejectFilter::run ()
{
    BandRejectFilter myFilter1(440, 200, 44100);
    runWithFilter(&myFilter1, 0.0);

    BandRejectFilter myFilter2(640, 200, 44100);
    runWithFilter(&myFilter2, 1.0);

    BandRejectFilter myFilter3(200, 100, 44100);
    runWithFilter(&myFilter3, 1.0);
}



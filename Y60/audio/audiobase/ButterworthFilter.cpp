//==============================================================================
// Copyright (c) 2002 ART+COM AG Berlin
// All rights reserved.
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//
//==============================================================================
//
// $Id: ButterworthFilter.cpp,v 1.2 2003/11/13 10:26:10 uzadow Exp $
// $Author: uzadow $
// $Revision: 1.2 $
// $Date: 2003/11/13 10:26:10 $
//
//
//==============================================================================

#include "ButterworthFilter.h"
#include "AudioBuffer.h"
#include "AudioInput.h"
#include "AudioOutput.h"


namespace AudioBase {

ButterworthFilter::ButterworthFilter(const char * theName, int mySampleRate, 
                double theVolume) :
    AudioSyncModule(theName, 1, mySampleRate, theVolume)
{
    newOutput(Mono);
    _myBuffer.x[0] = 0.0;
    _myBuffer.x[1] = 0.0;
    _myBuffer.y[0] = 0.0;
    _myBuffer.y[1] = 0.0;
}

ButterworthFilter::~ButterworthFilter() {
}

void
ButterworthFilter::process() {
    AudioBuffer * myInputBuffer = getInput(0)->get(); 
    SAMPLE * myData = myInputBuffer->getData();
    for (unsigned i = 0; i < myInputBuffer->getNumSamples(); ++i) {
        processSample(myData[i]);
    }
    getOutput(0)->put(*myInputBuffer);
    resetDeliveredInputs();
}

void
ButterworthFilter::processSample(double & theSample) {
    SAMPLE myOutputSample = a[0] * theSample + a[1] * _myBuffer.x[0] +
                            a[2] * _myBuffer.x[1] - b[0] * _myBuffer.y[0] -
                            b[1] * _myBuffer.y[1];

    _myBuffer.x[1] = _myBuffer.x[0];
    _myBuffer.x[0] = theSample;
    _myBuffer.y[1] = _myBuffer.y[0];
    _myBuffer.y[0] = myOutputSample;

    theSample = myOutputSample;
}

}

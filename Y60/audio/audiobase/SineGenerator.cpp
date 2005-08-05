
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: SineGenerator.cpp,v $
//
//     $Author: thomas $
//
//   $Revision: 1.9 $
//
//
// Description:
//
//
//=============================================================================

#include "SineGenerator.h"
#include "AudioScheduler.h"
#include "AudioOutput.h"
#include "AudioBuffer.h"

#include <cmath>

namespace AudioBase {

const double FREQUENCY = 440.0;
const double PI = 3.14159;

SineGenerator::SineGenerator(int mySampleRate)
        : AudioAsyncModule ("SineGenerator", mySampleRate, 1.0, false),
          _curSample(0),
          _myBuffersGenerated (0)
{
    _myOutput = newOutput(Mono);
}

SineGenerator::~SineGenerator() {
    _myOutput->stop();
}

bool
SineGenerator::init() {
    _myBuffer = new AudioBuffer();
    go();
    return true;
}

void
SineGenerator::process() {
    // Fill buffer.
    SAMPLE* myData = _myBuffer->getData();
    for (int i=0; i<_myBuffer->getNumSamples(); i++) {
        myData[i] = getSoftSin (_curSample+i);
    }
    _myOutput->put(*_myBuffer);
    _curSample += _myBuffer->getNumSamples();
    _myBuffersGenerated++;
}

double SineGenerator::getSoftSin (int theSampleNum ) {
    double myCurTime = double (theSampleNum)/getSampleRate();
    double mySin = sin(double(theSampleNum)*FREQUENCY/getSampleRate()*2*PI);
    if (myCurTime < 0.02) {
        // Fade in.
        mySin *= myCurTime*50;
    }
    return double (mySin);

}

}

//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: DummySoundCard.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.4 $
//
//
// Description: 
//
//
//=============================================================================

#include "DummySoundCard.h"
#include "AudioBuffer.h"

#include <asl/Logger.h>
#include <asl/Time.h>

namespace AudioBase {
    
using namespace std;

DummySoundCard::DummySoundCard ()
    : SoundCard()
{
    init("Dummy sound card", "Dummy sound card", 256, 256);
}

DummySoundCard::~DummySoundCard () {
}

void 
DummySoundCard::write(vector<AudioBuffer*>& theBuffers) {
    const double secsToSleep = AudioBuffer::getNumSamples()/double(getSampleRate());
    asl::msleep(int(secsToSleep*1000));
}

void 
DummySoundCard::read(vector<AudioBuffer*>& theBuffers) {
}

std::string
DummySoundCard::asString() const {
    return "--- Device: Dummy sound card ---";
}

int
DummySoundCard::getNumUnderruns() const {
    return 0;
}

double 
DummySoundCard::getCurrentTime() {
    asl::Time now;
    return double(now)-double(_myStartTime);
}

}

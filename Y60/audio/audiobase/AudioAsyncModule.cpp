

//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: AudioAsyncModule.cpp,v $
//
//     $Author: christian $
//
//   $Revision: 1.6 $
//
//
// Description: 
//
//
//=============================================================================

#include "AudioAsyncModule.h"
#include "AudioScheduler.h"


namespace AudioBase {

void 
AudioAsyncModule::go() {
    AudioScheduler::get().addModule(this);
}   

bool 
AudioAsyncModule::init() {
    return true;
}

void
AudioAsyncModule::pause() {
}

void
AudioAsyncModule::stop() {
    AudioScheduler::get().sendStop(this);
}

void
AudioAsyncModule::setSeekOffset(double theSeekOffset) {
}

}

//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: IAudioBufferListener.h,v $
//
//     $Author: david $
//
//   $Revision: 1.1 $
//
//=============================================================================

#ifndef INCL_AUDIOBUFFERLISTENER
#define INCL_AUDIOBUFFERLISTENER

namespace AudioBase {

    class AudioModule;
    class AudioBuffer;

    // Note: onNewData gets called in the context of the realtime thread 
    // that pushes the data into the sound card. Major processing should 
    // be done in another thread, not in this one.
    class IAudioBufferListener {
        public:
            virtual void onNewData(AudioModule* theModule, 
                    int theOutputIndex, 
                    AudioBuffer * theBuffer) = 0;
    };

}
#endif

//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: AudioSyncModule.h,v $
//
//     $Author: christian $
//
//   $Revision: 1.4 $
//
//
// Description: 
//
//
//=============================================================================

#ifndef INCL_AUDIOSYNCMODULE
#define INCL_AUDIOSYNCMODULE

#include <vector>

#include "AudioModule.h"
#include "AudioDefs.h"

namespace AudioBase {

class IAudioBufferListener;
class AudioInput;

class AudioSyncModule: public AudioModule {
    public:
        AudioSyncModule(const char * myName, int myNumChannels, 
                int mySampleRate, double myVolume); 
        virtual ~AudioSyncModule();

        virtual bool init() { return true; };

        virtual void nextInputDelivered(void);

        // This is called when an input gets disconnected by the source.
        // The function needs to be overridden only if you're expecting a
        // disconnect. Files will send disconnects, Mixers won't.
        virtual void inputDisconnected (AudioInput* theInput) {}; 

        int getNumInputs();
        virtual AudioInput * getInput(int i);
        virtual AudioInput * getInputById(const std::string& Id);
        virtual void dump (char * theIndent);
        AudioInput * newInput(ChannelType theType);

        virtual void registerListener(IAudioBufferListener* myListener);
        void callListeners(); 
        
    protected:
        AudioSyncModule(const AudioSyncModule &);
        
        void go();
        void deleteInput (AudioInput * theInput);
        int getNumConnectedInputs();
        void resetDeliveredInputs();
        std::vector<AudioInput *> _myInputs;

    private:    
        std::vector<IAudioBufferListener*> _myListener;
        int _myDeliveredInputs;
};

}

#endif

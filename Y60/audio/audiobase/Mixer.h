//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: Mixer.h,v $
//
//     $Author: thomas $
//
//   $Revision: 1.6 $
//
//
// Description: 
//
//
//=============================================================================

#ifndef INCL_MIXER
#define INCL_MIXER

#include "AudioSyncModule.h"
#include "AudioDefs.h"

#include <string>

namespace AudioBase {

class AudioInput;
class AudioOutput;
class AudioBuffer;
class NullSource;

class Mixer: public AudioSyncModule {

    public:
        Mixer(unsigned myNumChannels, int mySampleRate, const double myVolume);
        virtual ~Mixer();

        virtual void process();
        virtual void inputDisconnected (AudioInput* theInput);
        virtual const std::string getName();
        virtual bool init();

        AudioOutput * getOutput(ChannelType theOutput);
        virtual AudioInput * getInputById(const std::string& Id);

    private:
        void processChannel(unsigned channelIndex);
        std::vector<AudioOutput*> _myMixerOutputs;
        std::vector<AudioBuffer*> _myMixerBuffers;
        NullSource * _myNull;
        unsigned _myNumChannels;
};

}

#endif

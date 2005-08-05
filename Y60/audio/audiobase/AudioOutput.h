//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: AudioOutput.h,v $
//
//     $Author: ulrich $
//
//   $Revision: 1.4 $
//
//
// Description: 
//
//
//=============================================================================

#ifndef INCL_AUDIOOUTPUT
#define INCL_AUDIOOUTPUT

#include "AudioDefs.h"

namespace AudioBase {

class AudioModule;
class AudioInput;
class AudioBuffer;
class VolumeFader;

class AudioOutput {
public:
    AudioOutput(ChannelType myType, double myVolume, 
            VolumeFader * myFader);
    ~AudioOutput();

    void put(AudioBuffer& theData); 
    AudioModule* getInputModule();

    void connect(AudioInput* theInput);
    void disconnect();

    void stop();
    /**
     * \return the ChannelType of this channel as it was set at construction
     */
    ChannelType getType() const {
        return _myType;
    }
private:
    ChannelType _myType;
    AudioInput* _myConnectedInput;
    VolumeFader * _myFader;

    int _curSample;
};


}
#endif

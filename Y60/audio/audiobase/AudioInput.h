//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: AudioInput.h,v $
//
//     $Author: ulrich $
//
//   $Revision: 1.2 $
//
//
// Description: 
//
//
//=============================================================================

#ifndef INCL_AUDIOINPUT
#define INCL_AUDIOINPUT

#include "AudioDefs.h"

namespace AudioBase {

class AudioSyncModule;
class AudioOutput;
class AudioBuffer;

class AudioInput {
public:
    AudioInput(AudioSyncModule& myModule, ChannelType myType);
    ~AudioInput();
    
    void put(AudioBuffer& theData);
    AudioBuffer* get(void); 
    AudioSyncModule& getModule();
    ChannelType getType() const {
        return _myType;
    }

    void stop();
    void setConnected (bool theConnectFlag = true);
    bool isConnected ();
    
private:
    const ChannelType _myType;
    AudioSyncModule& _myModule;
    AudioBuffer* _myData;
    bool _isConnected;
};

}
#endif

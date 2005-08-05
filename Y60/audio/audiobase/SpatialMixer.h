//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: SpatialMixer.h,v $
//
//     $Author: christian $
//
//   $Revision: 1.7 $
//
//
// Description: 
//
//
//=============================================================================

#ifndef INCL_SPATIALMIXER
#define INCL_SPATIALMIXER

#include "AudioSyncModule.h"
#include "AudioDefs.h"

#include <string>
#include <vector>
#include <map>

#include <asl/Vector234.h>

namespace AudioBase {

class AudioInput;
class AudioOutput;
class AudioBuffer;
class NullSource;
class AudioScheduler;

class SpatialMixer: public AudioSyncModule {

    public:
        SpatialMixer(int numChannels, int mySampleRate, const double myVolume, 
             const asl::Vector2d & myExtents);
        virtual ~SpatialMixer();

        void setOutputPos(int myChannel, const asl::Vector2d & myPos);
        virtual void process();
        void setInputPos(AudioInput* theInput, const asl::Vector2d & myPos);

        virtual void inputDisconnected(AudioInput* theInput);
        virtual const std::string getName();
        virtual bool init();

        virtual AudioInput * getInputById(const std::string& Id);
        const asl::Vector2d& getExtents();
        void dump();
        
    private:
        void processChannel(AudioInput * curInput, 
                            int theOutputChannel,
                            double theChannelVolume);
        std::vector<double> getChannelVolumes(const asl::Vector2d & myPos);
        bool isCorrectChannelType 
                (AudioInput * theInput, 
                 int theChannelIndex, 
                 const asl::Vector2d & theInputPos);

        struct ChannelInfo {
            ChannelInfo(AudioOutput * myOutput);
            ~ChannelInfo();

            AudioOutput * _myOutput;
            AudioBuffer * _myBuffer;
            asl::Vector2d _myPos;
        };

        std::vector <ChannelInfo *> _myChannels;
        std::map <AudioInput*, asl::Vector2d> _myInputPosMap;

        NullSource * _myNull;
        asl::Vector2d _myExtents;
};

}

#endif

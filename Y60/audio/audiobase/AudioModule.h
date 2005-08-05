//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: AudioModule.h,v $
//
//     $Author: thomas $
//
//   $Revision: 1.10 $
//
//
// Description:
//
//
//=============================================================================

#ifndef INCL_AUDIOMODULE
#define INCL_AUDIOMODULE

#include "AudioDefs.h"
#include "VolumeFader.h"

#include <string>
#include <vector>
#include <list>

namespace AudioBase {

class AudioModule;
class AudioOutput;

typedef std::list<AudioModule*> ModuleList;

class AudioModule {
    public:
        AudioModule(const std::string & theName, int theSampleRate,
                    double theVolume, bool theFadeIn = false);
        virtual ~AudioModule();
        virtual void process(void) = 0;

        virtual bool init() { return true; }
        void postProcess ();
        virtual void setVolume (double myVolume);
        double getVolume ();
        virtual void fadeToVolume (double theVolume, double theDuration);

        ModuleList getConnectedModules();
        virtual const std::string getName();
        int getSampleRate() const;
        int getNumOutputs() const;
        AudioOutput* getOutput(int i);

        virtual void dump(char * theIndent="");

    protected:
        void setSampleRate(int theSampleRate);
        const VolumeFader& getFader() const;
        unsigned getCurSample() const;

        AudioOutput * newOutput (ChannelType theChannel);
        void deleteOutput (AudioOutput * theOutput);
        /**
         * Find an output channel by its type
         * @param theChannel type of the channel to retrieve
         * @return NULL if no such channel was found, otherwise the (first) channel
                   of the type theChannel
         */
        AudioOutput * findOutput(ChannelType theChannel) const;

    private:
        const std::string _myName;
        std::vector<AudioOutput*> _myOutputs;
        double _myVolume;
        VolumeFader _myFader;
        unsigned _curSample;
        int _mySampleRate;
};

}

#endif

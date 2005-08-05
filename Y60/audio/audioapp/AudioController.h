//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: AudioController.h,v $
//
//     $Author: christian $
//
//   $Revision: 1.19 $
//
//
// Description:
//
//
//=============================================================================

#ifndef INCL_AUDIOCONTROLLER
#define INCL_AUDIOCONTROLLER

#include <audio/AudioScheduler.h>

#include <asl/Singleton.h>
#include <asl/ITimeSource.h>
#include <asl/Time.h>
#include <asl/Point234.h>

#include <string>
#include <map>

namespace dom {
    class Node;
}

namespace AudioBase {
    class AudioModule;
    class AudioSyncModule;
    class AudioAsyncModule;
    class SoundOut;
    class Mixer;
    class IFileReader;
    class SoundCard;
}

namespace AudioApp {

class AudioController :
    public asl::ITimeSource,
    public asl::Singleton<AudioController>
{
    friend class asl::SingletonManager;
public:
    virtual ~AudioController();

    void init(unsigned int mySampleRate = 48000, double myLatency = 0.1, unsigned theNumChannels = 2);
    void init(const std::string& myConfigFile, double myLatency = 0.1);
    void init(const dom::Node& myConfig, double myLatency = 0.1);
    void deinit();

    std::string createReader(const std::string & theName,
                             const std::string & theDestModule,
                             unsigned theSampleRate, unsigned theNumChannels);

    std::string play(const std::string & theUrl,
                     const std::string & theDestModule,
                     double theVolume = 1.0,
                     double theBeginTime = 0,
                     bool theLoop = false);

    std::string play2d(const std::string & theUrl,
                       const std::string & theDestModule,
                       const asl::Point2d & thePosition,
                       double theVolume = 1.0,
                       double theBeginTime = 0,
                       bool theLoop = false);

    void stop();
    void stop(const std::string& theID);
    void pause(const std::string& theID);
    void setSeekOffset(const std::string& theID, double theSeekOffset);

    void setVolume(const std::string& theID, double theVolume);
    void fadeToVolume(const std::string& theID, double theVolume, double theDuration);
    double getVolume(const std::string& theID);
    double getDuration(const std::string& theID);
    bool isPlaying(const std::string& theID);
    void stopAllSounds();

    //static AudioController& getInstance();

    double getCurrentTime();
    double getCurrentDelay();
    int getNumUnderruns();
    double getLatency();

    int getNumSounds();

    int getSampleRate() const {
        return _mySoundCardSampleRate;
    }
    bool isRunning() const {
        return _myRunning;
    }

    AudioBase::IFileReader * getFileReaderFromID(const std::string& theID);

    typedef std::map<std::string, AudioBase::AudioModule*> ModuleMap;
    ModuleMap & getModules();

private:
    AudioController();

    // If this isn't there, gcc thinks noone calls the destructor and outputs
    // a bogus warning.
    friend class STOP_WARNING_ME_ABOUT_MY_PRIVATE_DESTRUCTOR;

    std::string createID(const std::string& theFileName);
    void connectModule(AudioBase::AudioModule & newModule, const dom::Node& curOutputNode);
    void createModule(const dom::Node& myNode);
    void createSoundCard(double theLatency, int theSampleRate, unsigned theNumChannels);
    void eraseSoundCard();

    std::string connectReader(const std::string & theFileName, AudioBase::IFileReader & theReader,
                              AudioBase::AudioSyncModule & theDestModule);
    AudioBase::IFileReader * createReaderForURL(const std::string & theUrl,
                                                double theVolume, double theBeginTime, bool theLoop);

    AudioBase::AudioModule * getModuleFromId(const std::string& theID);
    void syncModuleLists();

    bool _myRunning;
    bool _hasDefaultConfig; // True if no xml config given.

    // TODO: The module map still doesn't contain any modules that have been
    // added at runtime (e.g. via play()).
    ModuleMap _myModuleMap;

    typedef std::map<std::string, AudioBase::IFileReader*> ReaderMap;
    ReaderMap _myReaderMap;

    long long _myCurrentID;

    AudioBase::SoundCard * _mySoundCard;

    int _myMaxSounds;
    asl::Time _myStartTime;
    int _mySoundCardSampleRate;

    AudioBase::AudioScheduler * _myScheduler;
};

}

#endif

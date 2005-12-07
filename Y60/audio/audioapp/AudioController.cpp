//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: AudioController.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.52 $
//
//
// Description:
//
//
//=============================================================================

#include "AudioController.h"

#include <audio/SoundException.h>

#include <audio/BufferedSource.h>
#include <audio/WAVReader.h>
#include <audio/FFMpegAudioReader.h>
#include <audio/SoundOut.h>
#include <audio/SoundIn.h>
#include <audio/Mixer.h>
#include <audio/SpatialMixer.h>
#include <audio/TFilter.h>
#include <audio/SoundCard.h>
#include <audio/AudioAsyncModule.h>
#include <audio/Lowpass.h>
#include <audio/BandRejectFilter.h>
#include <audio/Difference.h>
#include <audio/SineGenerator.h>
#include <audio/AudioEater.h>
#include <audio/SoundCardManager.h>
#include <audio/SoundCard.h>
#include <audio/IAudioDecoderFactory.h>

#include <y60/DecoderManager.h>
#include <asl/Auto.h>
#include <asl/string_functions.h>
#include <asl/file_functions.h>
#include <asl/Point234.h>
#include <asl/ThreadHelper.h>
#include <asl/Dashboard.h>

#include <dom/Nodes.h>

#include <errno.h>
#ifdef LINUX
#include <unistd.h>
#endif
#include <pthread.h>

#define DB(x) // x

using namespace std;


namespace AudioApp {

using namespace AudioBase;
using namespace asl;
using namespace dom;

AudioController::AudioController() :
    _myRunning (false),
    _mySoundCard(0),
    _myCurrentID(0),
    _myScheduler(&AudioScheduler::get())
{}

AudioController::~AudioController() {
    DB(cerr << "AudioController::~AudioController()" << endl);
}

void
AudioController::stop() {
    deinit();
}

void
AudioController::createSoundCard(double theLatency, int theSampleRate, unsigned theNumChannels) {
#ifdef WIN32
    // Needs to be multiple of 1536 for AC3 encoding
    AudioBuffer::setSize(1536*6);
#else
    AudioBuffer::setSize(2048);
#endif

    SoundCardManager & theManager = SoundCardManager::get();
    _mySoundCard = theManager.getCard(0);
    _mySoundCard->setParams(theLatency, theSampleRate, 16, theNumChannels);
}

void
AudioController::init(unsigned int mySampleRate, double myLatency, unsigned theNumChannels) {
    if (_myRunning) {
        deinit();
    }
    _mySoundCardSampleRate = mySampleRate;
    createSoundCard(myLatency, _mySoundCardSampleRate, theNumChannels);

    SoundOut* mySoundOut = new SoundOut(_mySoundCard, theNumChannels, mySampleRate, 1.0);
    _myModuleMap["SoundOut"] = mySoundOut;

    AC_INFO << "Using driver " << _mySoundCard->getDeviceName() <<
        " for SoundCard " << _mySoundCard->getCardName() << "." << endl;

    Mixer * myMixer = new Mixer (_mySoundCard->getNumOutChannels(), mySampleRate, 0.3);
    myMixer->init();
    _myModuleMap["Mixer"] = myMixer;

    for (unsigned i = 0; i < myMixer->getNumOutputs(); ++i) {
        ChannelType type = getChannelType(i, myMixer->getNumOutputs());
        _myScheduler->connect(myMixer->getOutput(type), mySoundOut->getInput(type));
    }
    _myScheduler->start();
    _myRunning = true;
    _hasDefaultConfig = true;
    _myMaxSounds = 32;
}

void
AudioController::init(const string& myConfigFile, double myLatency)
{
    string s = asl::readFile(myConfigFile);
    if (s.length() == 0) {
        throw SoundException(
                std::string("AudioController::init() Error reading configuration file '")
                +myConfigFile+"'",PLUS_FILE_LINE);
    }

    AC_INFO << "AudioController: Reading config file " << myConfigFile << endl;
    dom::Document myDocument(s);
    dom::NodePtr myConfig = myDocument.childNode("AudioConfig");
    if (myConfig) {
        init(*myConfig, myLatency);
    } else {
        throw SoundException("Config error. Could not find node 'AudioConfig'.", PLUS_FILE_LINE);
    }
}

void
AudioController::init(const dom::Node& myConfigNode, double myLatency)
{
    DB(cerr << "AudioController::init()" << endl);
    if (_myRunning) {
        deinit();
    }

    if (myLatency < 0.001 && myConfigNode.getAttribute("latency")) {
        myLatency = myConfigNode.getAttributeValue<double>("latency");
    }

    _mySoundCardSampleRate = 48000;
    if (myConfigNode.getAttribute("sampleRate")) {
        _mySoundCardSampleRate = myConfigNode.getAttributeValue<int>("sampleRate");
    }

    unsigned myNumChannels = 2;
    if (myConfigNode.getAttribute("numChannels")) {
        myNumChannels = myConfigNode.getAttributeValue<int>("numChannels");
    }
    createSoundCard(myLatency, _mySoundCardSampleRate, myNumChannels);

    _myMaxSounds = 32;
    if (myConfigNode.getAttribute("maxSounds")) {
        _myMaxSounds = myConfigNode.getAttributeValue<int>("maxSounds");
    }

    for (int i = 0; i < myConfigNode.childNodesLength(); i++) {
        dom::NodePtr myModuleNode = myConfigNode.childNode(i);
        createModule(*myModuleNode);
    }

    AC_INFO << "Using driver for card " << _mySoundCard->getCardName() << "." << endl;

    // init the AudioModules
    ModuleMap::iterator it = _myModuleMap.begin();
    for(; it != _myModuleMap.end(); ++it) {
        (*it).second->init();
    }

    int myPriorityClass;
    int myPriority;

    string myPriorityClassStr = "SCHED_RR";
    myPriorityClassStr = myConfigNode.getAttributeString("priorityClass");

    string myPriorityStr = "0";
    myPriorityStr = myConfigNode.getAttributeString("priority");
    bool ok = threadPriorityFromStrings (myPriorityClassStr, myPriorityStr,
            myPriorityClass, myPriority);
    if (!ok) {
        AC_WARNING << "AudioController::init() Couldn't parse priority. Using default" << endl;
        myPriorityClass = SCHED_RR;
        myPriority = 0;
    }

    _myScheduler->start(myPriorityClass, myPriority);
    _myRunning = true;
    _hasDefaultConfig = false;
}

void
AudioController::connectModule(AudioModule & newModule, const dom::Node& curOutputNode) {
    int myChannel = curOutputNode.getAttributeValue<int>("channel");
    string myDest = curOutputNode.getAttributeString("dest");
    string::size_type myDelimPos = myDest.find('.');

    string myDestModuleId;
    string myDestChannelId;
    if (myDelimPos == string::npos) {
        myDestModuleId = myDest;
        myDestChannelId = "";
    } else {
        myDestModuleId = myDest.substr(0, myDelimPos);
        myDestChannelId = myDest.substr(myDelimPos+1);
    }

    ModuleMap::iterator destModuleIt = _myModuleMap.find(myDestModuleId);
    if (destModuleIt != _myModuleMap.end()) {
        AudioSyncModule * destModule = dynamic_cast<AudioSyncModule*>(destModuleIt->second);
        AC_DEBUG << "AudioController::connectModule():  Connect: " << newModule.getName()
            << "." << myChannel << " with " << myDest << endl;
        AudioInput * myInput = destModule->getInputById(myDestChannelId);
        if (myInput) {
            _myScheduler->connect(newModule.getOutput(myChannel), myInput);

            // Special case for SpatialMixer: Output node contains
            // speaker positions. Can't put this code into SpatialMixer
            // or we'll introduce an xml dependency into libAudioBase.
            SpatialMixer *mySpatialMixer = dynamic_cast<SpatialMixer*>(&newModule);
            if (mySpatialMixer) {
                asl::Point2d myPos(curOutputNode.getAttributeValue<asl::Point2d>("pos"));
                mySpatialMixer->setOutputPos(myChannel, myPos);
            }

        } else {
            throw SoundException(
                    string("Configuration error: Destination module\n")
                    + myDestModuleId + " doesn't have an input channel called "
                    + myDestChannelId + ".", PLUS_FILE_LINE);
        }
    } else {
        throw SoundException(string("Configuration error: Output node\n")
                + as_string(curOutputNode) +
                "has unknown destination " + myDest, PLUS_FILE_LINE);
    }
}

void
AudioController::createModule(const dom::Node& myNode) {
    if (myNode.nodeType() == dom::Node::COMMENT_NODE) {
        return;
    }
    double myVolume = 1.0;
    if (myNode.getAttribute("volume")) {
        myVolume = myNode.getAttributeValue<double>("volume");
    }
    string myId = myNode.getAttributeString("id");
    string myNodeName = myNode.nodeName();

    AudioModule * newModule = 0;
    AC_DEBUG << "Processing " << myNode.nodeName() << endl;
    if (myNodeName == "StereoMixer") {
        int myChannels = 2;
        newModule = new Mixer(myChannels, _mySoundCardSampleRate, myVolume);
    } else if (myNodeName == "#comment") {
        return;
    } else if (myNodeName == "SoundOut") {
        int myChannels = myNode.getAttributeValue<int>("numChannels");
        newModule = new SoundOut(_mySoundCard, myChannels, _mySoundCardSampleRate, myVolume);
    } else if (myNodeName == "AudioEater") {
        int myChannels = myNode.getAttributeValue<int>("numChannels");
        newModule = new AudioEater(myChannels, _mySoundCardSampleRate);
    } else if (myNodeName == "SoundIn") {
        int myChannels = myNode.getAttributeValue<int>("numChannels");
        newModule = new SoundIn(_mySoundCard, myChannels, _mySoundCardSampleRate, myVolume);
    } else if (myNodeName == "SpatialMixer") {
        int myChannels = myNode.getAttributeValue<int>("numChannels");
        asl::Point2d myExtent (myNode.getAttributeValue<asl::Point2d>("extent"));
        newModule = new SpatialMixer(myChannels, _mySoundCardSampleRate, myVolume, myExtent);
    } else if (myNodeName == "TFilter") {
        int myChannels = myNode.getAttributeValue<int>("numChannels");
        newModule = new TFilter(myChannels, _mySoundCardSampleRate, myVolume);
    } else if (myNodeName == "Difference") {
        newModule = new Difference(_mySoundCardSampleRate, myVolume);
    } else if (myNodeName == "Lowpass") {
        int myFrequency = myNode.getAttributeValue<int>("frequency");
        newModule = new Lowpass(myFrequency, _mySoundCardSampleRate, myVolume);
    } else if (myNodeName == "BandRejectFilter") {
        int myFrequency = myNode.getAttributeValue<int>("frequency");
        int myBandwidth = myNode.getAttributeValue<int>("bandwidth");
        newModule = new BandRejectFilter(myFrequency, myBandwidth, _mySoundCardSampleRate, myVolume);
    } else if (myNodeName == "SineGenerator") {
        SineGenerator * newSineGenerator = new SineGenerator(_mySoundCardSampleRate);
        newModule = newSineGenerator;
    } else {
        AC_WARNING << "AudioController warning: Unknown Module type " << myNodeName
            << " in configuration file ignored." << endl;
        return;
    }

    if (newModule) {
        if (myId == "") {
            throw SoundException("Configuration error: ID field missing for\n" + myNode,
                    PLUS_FILE_LINE);
        }

        _myModuleMap[myId] = newModule;

        AC_DEBUG << "AudioController:: Adding module "<< myId << " to graph." << endl;
        for (int i = 0; i < myNode.childNodesLength("Output"); i++) {
            dom::NodePtr curOutputNode = myNode.childNode("Output", i);
            connectModule (*newModule, *curOutputNode);

        }
    }
}


void
AudioController::deinit() {
    DB(cerr << "AudioController::deinit()" << endl);
    syncModuleLists();
    if (_myRunning) {
        _myScheduler->stop();
        syncModuleLists();
        while (!_myModuleMap.empty()) {
            DB(_myModuleMap.begin()->second->dump());

            // [CH] Not a good idea, makes windows crash and causes linux heap curruption
            //DB(cerr << "AudioController deleting '" << typeid(*(_myModuleMap.begin()->second)).name() << endl;)
            //delete _myModuleMap.begin()->second;
            _myModuleMap.erase(_myModuleMap.begin());
        }
    }
    _myRunning = false;
}

std::string
AudioController::connectReader(const std::string & theFileName, IFileReader & theReader, AudioSyncModule & theDestModule) {
    AutoLocker<AudioScheduler> myLocker(*_myScheduler);
    // Init needs to be called before play.
    if (!_myRunning) {
        cerr << "### WARNING: AudioController::connectReader() called before init. Ignoring." << endl;
        return "error";
    }

    string myID = createID(theFileName);
    _myReaderMap[myID] = &theReader;

    for (unsigned i = 0; i < theReader.getNumChannels(); ++i) {
        ChannelType type = getChannelType(i, theReader.getNumChannels());
        _myScheduler->connect(theReader.getOutput(type), theDestModule.newInput(type));
    }
    /*
    if (theReader.getNumChannels() == 1) {
        _myScheduler->connect(theReader.getOutput(0), theDestModule.newInput(Mono));
    } else {
        _myScheduler->connect(theReader.getOutput(0), theDestModule.newInput(Left));
        _myScheduler->connect(theReader.getOutput(1), theDestModule.newInput(Right));
    }
    */
    return myID;
}

std::string
AudioController::createReader(const std::string & theName,
                              const std::string & theDestModule,
                              unsigned theSampleRate, unsigned theNumChannels)
{
    syncModuleLists();
    if (_myReaderMap.size() > _myMaxSounds) {
        cerr << "### WARNING : AudioController::createReader(): More than " << _myMaxSounds << " sounds playing. Ignoring request." << endl;
        return "-1";
    }

    Mixer * myDestModule = dynamic_cast<Mixer*>(getModuleFromId(theDestModule));
    if (!myDestModule) {
        throw SoundException(string("Module ID ")+theDestModule+" is not a mixer.",
                PLUS_FILE_LINE);
    }
    BufferedSource * myBufferedSource = new BufferedSource(theName);
    myBufferedSource->setup(theNumChannels, theSampleRate);
	myBufferedSource->go();
    return connectReader(theName, *myBufferedSource, *myDestModule);
}

AudioBase::IFileReader *
AudioController::createReaderForURL(const std::string & theUrl,
                                    double theVolume,
                                    double theBeginTime,
                                    bool theLoop)
{
    AudioBase::IFileReader * myReader = 0;

    // First: Look for registered decoders that could handle the source
    Ptr<IAudioDecoderFactory> myDecoderFactory = y60::DecoderManager::get().findDecoder<IAudioDecoderFactory>(theUrl);

    if (myDecoderFactory) {
        myReader = myDecoderFactory->instance(theUrl, theVolume);
        if (!myReader) {
            throw SoundException(string("Could not create instance for reader for url: ") + theUrl, PLUS_FILE_LINE);
        }
    } else {
        // Second: Try wav decoder by extension
        std::string myExtension = asl::toLowerCase(asl::getExtension(theUrl));
        if (myExtension == "wav") {
            //cerr << "### WARNING : WAVReader doesn't support resampling... '" << theUrl << "'" << endl;
            myReader = new WAVReader(theUrl, _mySoundCardSampleRate, theVolume, theBeginTime, theLoop);
        } else if (myExtension == "mp3") {
            myReader = new FFMpegAudioReader(theUrl, theVolume, (theLoop ? 0 : 1));
        } else {
            // Use FFMepg as fall back.
            myReader = new FFMpegAudioReader(theUrl, theVolume, (theLoop ? 0 : 1));
        }
    }
    return myReader;
}

std::string
AudioController::play (const string & theUrl,
                       const string & theDestModule,
                       double theVolume,
                       double theBeginTime,
                       bool theLoop)
{
    syncModuleLists();
    if (_myReaderMap.size() > _myMaxSounds) {
        cerr << "### WARNING : AudioController::play(): More than " << _myMaxSounds << " sounds playing. Ignoring request." << endl;
        return "-1";
    }

    Mixer * myDestModule = dynamic_cast<Mixer*>(getModuleFromId(theDestModule));
    if (!myDestModule) {
        throw SoundException(string("Module ID ")+theDestModule+" is not a mixer.", PLUS_FILE_LINE);
    }

    IFileReader * myReader = createReaderForURL(theUrl, theVolume, theBeginTime, theLoop);
	std::string myReaderId("-1");
	if (myReader) {
		myReaderId = connectReader(theUrl, *myReader, *myDestModule);
	}
	return myReaderId;
}


std::string
AudioController::play2d(const string & theUrl,
                        const string & theDestModule,
                        const asl::Point2d & thePosition,
                        double theVolume,
                        double theBeginTime,
                        bool theLoop)
{
    syncModuleLists();
    if (_myReaderMap.size() > _myMaxSounds) {
        cerr << "### WARNING : AudioController::play2d(): More than " << _myMaxSounds << " sounds playing. Ignoring request." << endl;
        return "-1";
    }

    SpatialMixer * myDestModule = dynamic_cast<SpatialMixer*>(getModuleFromId(theDestModule));
    if (!myDestModule) {
        throw SoundException(
                string("play2d: Module ID ")+theDestModule+" is not a SpatialMixer.",
                PLUS_FILE_LINE);
    }

    IFileReader * myReader = createReaderForURL(theUrl, theVolume, theBeginTime, theLoop);
    std::string myId = connectReader(theUrl, *myReader, *myDestModule);
    myDestModule->setInputPos(myDestModule->getInput(0), thePosition);
    myDestModule->setInputPos(myDestModule->getInput(1), thePosition);

    return myId;
}


void
AudioController::pause (const std::string & theID) {
    IFileReader * myReader = getFileReaderFromID(theID);
    if (myReader) {
        myReader->pause();
    }
}

void
AudioController::stop (const std::string & theID) {
    MAKE_SCOPE_TIMER(AudioController_stop);
    IFileReader * myReader = getFileReaderFromID(theID);
    if (myReader) {
        myReader->stop();
        ReaderMap::iterator it = _myReaderMap.find(theID);
        _myReaderMap.erase(it);
    }
}

void
AudioController::setSeekOffset(const std::string& theID, double theSeekOffset) {
    IFileReader * myReader = getFileReaderFromID(theID);
    if (myReader) {
        myReader->setSeekOffset(theSeekOffset);
    }
}

void
AudioController::setVolume (const std::string& theID, double theVolume) {
    MAKE_SCOPE_TIMER(AudioController_setVolume);
    AudioModule * myModule = getModuleFromId(theID);
    myModule->setVolume(theVolume);
}

void
AudioController::fadeToVolume(const std::string& theID, double theVolume,
        double theDuration)
{
    MAKE_SCOPE_TIMER(AudioController_fadeToVolume);
    AudioModule * myModule = getModuleFromId(theID);
    myModule->fadeToVolume(theVolume, theDuration);
}

double
AudioController::getVolume(const std::string& theId) {
    AudioModule * myModule = getModuleFromId(theId);
    return myModule->getVolume();
}

double
AudioController::getDuration (const std::string& theID) {
    IFileReader * myReader = getFileReaderFromID(theID);
    if (myReader) {
        return myReader->getDuration();
    }
    return -1.0;
}

bool
AudioController::isPlaying(const std::string& theID) {
    syncModuleLists();
    return (_myReaderMap.find(theID) !=_myReaderMap.end());
}

void
AudioController::stopAllSounds() {
    MAKE_SCOPE_TIMER(AudioController_stopAllSounds);
    syncModuleLists();
    ReaderMap::iterator it;
    for (it = _myReaderMap.begin(); it != _myReaderMap.end(); ++it) {
        IFileReader * curModule = it->second;
        curModule->stop();
    }
    _myReaderMap.clear();
}


double
AudioController::getCurrentTime () {
    return _mySoundCard->getCurrentTime();
}

double
AudioController::getCurrentDelay() {
    return _mySoundCard->getCurrentDelay();
}

int
AudioController::getNumUnderruns() {
    return _mySoundCard->getNumUnderruns();
}

double
AudioController::getLatency() {
    return _mySoundCard->getLatency();
}

AudioController::ModuleMap &
AudioController::getModules() {
    return _myModuleMap;
}

IFileReader *
AudioController::getFileReaderFromID(const std::string& theID) {
    syncModuleLists();
    ReaderMap::iterator it = _myReaderMap.find(theID);
    if (it == _myReaderMap.end()) {
        AC_ERROR << "AudioController: No such ID '" << theID << "'";
        return NULL;
    }
    return it->second;
}

AudioModule *
AudioController::getModuleFromId(const std::string& theID) {
    syncModuleLists();
    AudioModule *myModule;
    ReaderMap::iterator it = _myReaderMap.find(theID);
    if (it != _myReaderMap.end()) {
        myModule = it->second;
    } else {
        ModuleMap::iterator it = _myModuleMap.find(theID);
        if (it != _myModuleMap.end()) {
            myModule = it->second;
        } else {
            throw SoundException(string("Module ID ")+theID+" is unknown.",
                    PLUS_FILE_LINE);
        }
    }
    return myModule;
}

void
AudioController::syncModuleLists() {
    // TODO: This function is there because there are two lists of modules -
    // one in the AudioController and one in the AudioScheduler - that get out
    // of sync whenever a module stops by itself (for instance, if a wave file
    // is played to the end). As such, it's a cludge that would disappear if
    // we only had one list...
    AsyncModuleList myStoppedList;
    _myScheduler->getAndClearStoppedList(myStoppedList);
    AsyncModuleList::iterator it;
    // Warning: myStoppedList is full of broken pointers. Don't dereference.
    // TODO: Change this.
    for (it = myStoppedList.begin(); it != myStoppedList.end(); ++it) {
        ReaderMap::iterator it2 ;
        for (it2 = _myReaderMap.begin(); it2 != _myReaderMap.end(); ++it2) {
            IFileReader * curModule = it2->second;
            if (curModule == *it) {
                _myReaderMap.erase(it2);
                break;
            }
        }
    }
}

int
AudioController::getNumSounds() {
    return _myScheduler->getNumAsyncModules();
}

string
AudioController::createID (const string& theFileName) {
    return theFileName+"_"+asl::as_string (_myCurrentID++);

}

}

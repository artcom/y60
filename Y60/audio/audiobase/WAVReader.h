//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: WAVReader.h,v $
//
//     $Author: ulrich $
//
//   $Revision: 1.9 $
//
//
// Description: 
//
//
//=============================================================================

#if !defined (INCL_WAVREADER)
#define INCL_WAVREADER

#include "AudioAsyncModule.h"
#include "IFileReader.h"
#include "AudioBuffer.h"

#include <string>
#include <vector>
#include <fstream>
#include <iosfwd>

namespace AudioBase {

class AudioOutput;
class AudioBuffer;

struct WAVHeader{
    asl::Signed32	magic;			// 'RIFF' 
    asl::Signed32	len0;			// Chunk size = len + 8 + 16 + 12 
    asl::Signed32	magic1;			// 'WAVE' 
    asl::Signed32	magic2;			// 'fmt ' 
    asl::Signed32	len;			// length of samples 
    asl::Signed16	format;			// 1 is PCM, rest not known 
    asl::Signed16	nchns;			// Number of channels 
    asl::Signed32	rate;			// sampling frequency 
    asl::Signed32	aver;			// Average bytes/sec !! 
    asl::Signed16	nBlockAlign;    // (rate*nch +7)/8 
    asl::Signed16	size;			// size of each sample (8,16,32) 
};

class WAVReader: public IFileReader {
public:
    WAVReader (const std::string & myFName, 
               int mySampleRate, double myVolume = 1.0, double myStartTime = 0.0, bool myRepeat = false);

    virtual void stop ();
    unsigned getNumChannels() const;

    AudioOutput * getOutput (ChannelType theType);
    double getDuration () const;
    virtual void dump (char * theIndent);

    // Interface to Scheduler.
    virtual void process ();
    virtual bool init();

private:    
    virtual ~WAVReader ();

    // Prevent bogus gcc warning.
    friend class PREVENT_PRIVATE_DESTRUCTOR_WARNING;
    
    void readWAVHeader (std::ifstream & theInFile);

    AudioOutput * _myLeftOutput;
    AudioOutput * _myRightOutput;
    
    std::ifstream::pos_type _myDataLen;

    std::string _myFName;
    std::ifstream _myInFile;
    bool _myEOF;
    int _myNumChannels;
    int _mySampleSize;
    WAVHeader _myHeader;
    std::ifstream::pos_type _myDataStart;
    std::vector<char> _myFileSamples;
    AudioBuffer *_myLeftBuffer; // Also used for mono
    AudioBuffer *_myRightBuffer;

    int getCurSample ();
    bool setCurSample (std::ifstream::pos_type thePos);

    double _myStartTime;
    int _myStopSample; // Sample when the stop signal was received.
    int _mySamplesPlayed;    
    enum State {RUNNING, STOPPING, STOPPED};
    State _myState;
    bool _myRepeat;
};

}
#endif


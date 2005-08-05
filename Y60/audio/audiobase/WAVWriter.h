//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: WAVWriter.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.4 $
//
//
// Description: 
//
//
//=============================================================================

#ifndef INCL_WAV_WRITER
#define INCL_WAV_WRITER

#include "AudioSyncModule.h"
//#include <stdio.h>
#include <iosfwd>
#include <fstream>

namespace AudioBase {

class SoundCard;
class AudioBuffer;

class WAVWriter: public AudioSyncModule {
public:
    struct WaveChunkHeader {
	    unsigned int type;		/* 'data' */
    	unsigned int length;		/* samplecount */
    };
    struct WaveHeader{
	    unsigned int magic;		/* 'RIFF' */
    	unsigned int length;		/* filelen */
    	unsigned int type;		/* 'WAVE' */
    };

    struct WaveFmtBody{
        asl::Unsigned16 format;		/* should be 1 for PCM-code */
        asl::Unsigned16 modus;		/* 1 Mono, 2 Stereo */
        unsigned int sample_fq;	/* frequence of sample */
        unsigned int byte_p_sec;
        asl::Unsigned16 byte_p_spl;	/* samplesize; 1 or 2 bytes */
        asl::Unsigned16 bit_p_spl;	/* 8, 12 or 16 bit */
    };

    WAVWriter (const std::string & myFName, int mySampleRate, int numChannels=2);
    virtual ~WAVWriter ();

    // audiosyncmodule interface
    virtual void process ();
    virtual void inputDisconnected (AudioInput* theInput);
    virtual bool init();


private:
    void createWAVHeader(int myLength);
    void writeBuffer(const std::vector<AudioBuffer*> & myBuffer);
    void finalize();

    std::string _myFileName;
    std::ofstream   _myOutFile; 
    // int _myFileDescriptor;

    asl::Signed64 _myByteWritten;
    bool      _myFinalized;
    int       _myNumChannels;
};

}
#endif

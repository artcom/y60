//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: WAVWriter.cpp,v $
//
//     $Author: christian $
//
//   $Revision: 1.6 $
//
//
// Description: 
//
//
//=============================================================================

#include "WAVWriter.h"
#include "WAVReader.h"

#include "AudioScheduler.h"
#include "AudioBuffer.h"
#include "SoundException.h"
#include "AudioInput.h"
#include "SoundCard.h"
#ifdef LINUX
#include <alsa/asoundlib.h>
#include <byteswap.h>
#endif

#include <iostream>
#include <iomanip>
#include <math.h>

#define COMPOSE_ID(a,b,c,d)	((a) | ((b)<<8) | ((c)<<16) | ((d)<<24))
#define WAV_RIFF		COMPOSE_ID('R','I','F','F')
#define WAV_WAVE		COMPOSE_ID('W','A','V','E')
#define WAV_FMT			COMPOSE_ID('f','m','t',' ')
#define WAV_DATA		COMPOSE_ID('d','a','t','a')
#define WAV_PCM_CODE		1

namespace AudioBase {
    
using namespace std;
using namespace asl;


WAVWriter::WAVWriter (const std::string & myFName, int mySampleRate, int numChannels)
    : AudioSyncModule(string("WAVWriter: " + myFName).c_str() ,numChannels, 
            mySampleRate, 1.0),
      _myFileName(myFName), 
      _myByteWritten(0), _myNumChannels(numChannels),
      _myFinalized(false)
{
}

WAVWriter::~WAVWriter () {
}

bool
WAVWriter::init() {

    remove(_myFileName.c_str());
    _myFinalized = false;
    _myOutFile.exceptions(ios::failbit | ios::badbit);
    try {
        _myOutFile.open(_myFileName.c_str(), ios::binary);
    } catch (const ios::failure & ex) {
        throw SoundException(string("WAVWriter::init(): stream error while creating '")+
                _myFileName+"': "+ex.what(),PLUS_FILE_LINE);
    }
    createWAVHeader(32768);
    return true;
}


void 
WAVWriter::process () {

    vector<AudioBuffer*> myBuffer;
    for (int i = 0; i < _myNumChannels; i++) {
        myBuffer.push_back(getInput(i)->get());
    }

    writeBuffer(myBuffer);
    
    resetDeliveredInputs();
}

void 
WAVWriter::inputDisconnected (AudioInput* theInput) {
    if (!_myFinalized) {
        finalize();
    }
    deleteInput (theInput);
}

void
WAVWriter::createWAVHeader(int myLength) {
	WaveHeader h;
	WaveFmtBody f;
	WaveChunkHeader cf, cd;
	int bits;
	unsigned int tmp;
    asl::Unsigned16 tmp2;

	bits = 16;

	h.magic = WAV_RIFF;
	tmp = myLength + sizeof(WaveHeader) + sizeof(WaveChunkHeader) + 
                     sizeof(WaveFmtBody) + sizeof(WaveChunkHeader) - 8;
	h.length = tmp;
	h.type = WAV_WAVE;

	cf.type = WAV_FMT;
	cf.length = 16;

	f.format = WAV_PCM_CODE;
	f.modus = _myNumChannels;
	f.sample_fq = getSampleRate();
	tmp2 =  _myNumChannels * ((bits + 7) / 8);
	f.byte_p_spl = tmp2;
	tmp = (unsigned int) tmp2 * getSampleRate();
	f.byte_p_sec = tmp;
	f.bit_p_spl = bits;

	cd.type = WAV_DATA;
	cd.length = myLength;

    try {
    	_myOutFile.write(reinterpret_cast<const char *>(&h), sizeof(WaveHeader));
        _myOutFile.write(reinterpret_cast<const char *>(&cf), sizeof(WaveChunkHeader));
        _myOutFile.write(reinterpret_cast<const char *>(&f), sizeof(WaveFmtBody));
    	_myOutFile.write(reinterpret_cast<const char *>(&cd), sizeof(WaveChunkHeader));
    } catch (const ios::failure & ex) {
        throw SoundException(string("WAVWriter::createWAVHeader(): stream error :")+
                ex.what(),PLUS_FILE_LINE);
    }
}

void 
WAVWriter::writeBuffer(const vector<AudioBuffer*> & myBuffer) { 
    unsigned myBufferSize = AudioBuffer::getNumSamples();
    for (int sample = 0; sample < myBufferSize; sample++) {
        for (int buffer = 0; buffer < _myNumChannels; buffer++) {
            asl::Signed16 theData = myBuffer[buffer]->getData16(sample);
            try {
                _myOutFile.write(reinterpret_cast<const char *>(&theData), 2);
            } catch (const ios::failure & ex) {
                throw SoundException(string("WAVWriter::writeBuffer(): stream error :")+
                        ex.what(),PLUS_FILE_LINE);
            }
        }
    }
   
    _myByteWritten += 2 * AudioBuffer::getNumSamples() * _myNumChannels;
}

void 
WAVWriter::finalize()
{		
	WaveChunkHeader cd;
    std::ios::pos_type length_seek = sizeof(WaveHeader) +
                        sizeof(WaveChunkHeader) +
                        sizeof(WaveFmtBody);
    cd.type = WAV_DATA;
	cd.length = _myByteWritten > 0xffffffff ? 0xffffffff : static_cast<int>(_myByteWritten);
    try {
        _myOutFile.seekp(length_seek, ios::beg);
		_myOutFile.write(reinterpret_cast<const char*>(&cd), sizeof(WaveChunkHeader));
		_myOutFile.close();
    } catch (const ios::failure & ex) {
        throw SoundException(string("WAVWriter::finalize(): stream error :")+
                ex.what(),PLUS_FILE_LINE);
    }
/*    AC_DB("WAV File : " << _myFileName.c_str() 
         << " written with " << _myByteWritten << " bytes. ");
*/         
    _myFinalized = true;
}
}

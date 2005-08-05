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
//    $RCSfile: WAVReader.cpp,v $
//
//     $Author: build $
//
//   $Revision: 1.19 $
//
//
// Description: 
//
//
//=============================================================================

#include "WAVReader.h"

#include "SoundException.h"
#include "AudioScheduler.h"
#include "AudioDefs.h"
#include "AudioOutput.h"
#include "AudioBuffer.h"

#include <asl/Logger.h>
#include <asl/Assure.h>
#include <asl/string_functions.h>

#include <stdio.h>
#include <errno.h>
#include <string>

namespace AudioBase {

const char     RIFF_ID[4] = {'R','I','F','F'};
const char     WAVE_ID[4] = {'W','A','V','E'};
const char     FMT_ID[4]  = {'f','m','t',' '};
const char     DATA_ID[4] = {'d','a','t','a'};

struct wave_data {
  asl::Signed32	magic3;			// 'data' 
  asl::Signed32	datasize;		// data chunk size 
};

WAVReader::WAVReader (const std::string & myFName, 
                      int mySampleRate, 
                      double myVolume, 
                      double myStartTime, 
                      bool myRepeat)
    : IFileReader(std::string("WAVReader: " + myFName), mySampleRate, myVolume, (myStartTime != 0.0)), 
    _myLeftOutput(0), _myRightOutput(0),
    _myDataLen(0),
    _myFName(myFName),
    _myEOF(false),
    _myNumChannels(0),
    _mySampleSize(0),
    _myDataStart(0),
    _myLeftBuffer(0), _myRightBuffer(0),
    _myStartTime(myStartTime), 
    _myStopSample(-1), 
    _mySamplesPlayed(0),
    _myState(RUNNING),
    _myRepeat(myRepeat)
{
    AC_DEBUG << "WAVReader " << (void*)this;
    init();
}

WAVReader::~WAVReader () {
    AC_DEBUG << "~WAVReader " << (void*)this;
    if (_myInFile.is_open()) {
        _myInFile.close();
    }

    if (_myLeftOutput) {
        _myLeftOutput->stop();
    }
    if (_myLeftBuffer) {
        delete _myLeftBuffer;
        _myLeftBuffer = 0;
    }
    if (_myRightOutput) {
        _myRightOutput->stop();
    }
    if (_myRightBuffer) {
        delete _myRightBuffer;
        _myRightBuffer = 0;
    }
}

bool WAVReader::init() {
    try {
        _myInFile.exceptions(std::ios::failbit | std::ios::badbit);
        try {
            _myInFile.open(_myFName.c_str(), std::ios::in | std::ios::binary);
        } catch (const std::ios::failure & ex) {
            throw SoundException(std::string("WAVReader::init(): stream error :")+ex.what(),PLUS_FILE_LINE);
        }
        readWAVHeader (_myInFile);
        
        if (getNumChannels() == 1) {
            _myLeftOutput = newOutput(Mono);
            _myLeftBuffer = new AudioBuffer();   
        } else {
            _myLeftOutput = newOutput(Left);
            _myLeftBuffer = new AudioBuffer();
            _myRightOutput = newOutput(Right);
            _myRightBuffer = new AudioBuffer();
        }
        _myFileSamples.resize(AudioBuffer::getNumSamples()*3*2); // Enough for 24 bit stereo;

        if (_myStartTime != 0.0) {
            int myStartSample = int (_myStartTime*getSampleRate());
            bool myStartOK = setCurSample (myStartSample);
            if (!myStartOK) {
                AC_WARNING << "WAVReader::init(): Start time > file length in " 
                     << _myFName << ".";
                _myState = STOPPED;
                AudioScheduler::get().sendStop(this);
                return false;
            }
        }

        _myEOF = false;
        go();
    } catch (...) {
        if (_myInFile.is_open()) {
            _myInFile.close();
        }
        throw;
    }
    return AudioAsyncModule::init();
}

void 
WAVReader::stop () {
    switch (_myState) {
        case RUNNING:
            _myState = STOPPING;
            setVolume (0);
            _myStopSample = _mySamplesPlayed+_myLeftBuffer->getNumSamples();
            AC_DEBUG << "WAVReader::stop(): setVolume (0), myStopSample="<<_myStopSample
                 <<" [" << this << "]";
            break;
        case STOPPING:
            break;
        case STOPPED:
            break;
        default:
            throw(SoundException("WAVReader::stop: illegal state.", PLUS_FILE_LINE));
            break;
    }
}

unsigned
WAVReader::getNumChannels () const {
    return _myNumChannels;
}

AudioOutput * 
WAVReader::getOutput (ChannelType theType) {
    if (_myNumChannels == 1) {
        
        return _myLeftOutput;
    } else {
        switch (theType){
            case Left: 
                return _myLeftOutput;
                break;
            case Right:
                return _myRightOutput;
                break;
            default:
                // There is no mono output in a stereo wav file.
                ASSURE(false);
                return 0;
        }
    }
}

double 
WAVReader::getDuration () const {
    if (_myRepeat) {
        return -1.0;
    } else {
        return double(_myDataLen-_myDataStart)/
                     (_myNumChannels*getSampleRate()*_mySampleSize/8);
    }
}

void 
WAVReader::dump (char * theIndent) {
    AudioModule::dump(theIndent);
    AC_TRACE << theIndent << "  _myDataLen: " << _myDataLen;
    // DUMP_VARIABLE(_myInFile);
    AC_TRACE_EXPRESSION(_myEOF);
    AC_TRACE_EXPRESSION(_myNumChannels);
    AC_TRACE_EXPRESSION(_mySampleSize);
    AC_TRACE_EXPRESSION(_myDataStart);
    AC_TRACE_EXPRESSION(_myLeftBuffer);
    AC_TRACE_EXPRESSION(_myRightBuffer);
    AC_TRACE_EXPRESSION(_myStartTime);
    AC_TRACE_EXPRESSION(_myStopSample);
    AC_TRACE_EXPRESSION(_mySamplesPlayed);
    AC_TRACE_EXPRESSION(_myState);
    AC_TRACE_EXPRESSION(_myRepeat);
}

void 
WAVReader::process () {
    try {
        std::streamsize myBytesPerBuffer = _mySampleSize/8*_myLeftBuffer->getNumSamples()*_myNumChannels;
        std::streamsize myBytesToRead = myBytesPerBuffer;
        bool isLastBuffer = false;
        int myBytesLeft = _myDataStart+_myDataLen-_myInFile.tellg();
        if (myBytesLeft < myBytesToRead) {
            isLastBuffer = true;
            myBytesToRead = myBytesLeft;
        }
        _myInFile.read (&(*_myFileSamples.begin()), myBytesToRead);
        std::streamsize myBytesRead = _myInFile.gcount();
        if (myBytesRead != myBytesToRead) {
            AC_WARNING << "trying to read " << myBytesToRead << ", but only got " << myBytesRead << ". Truncated wav file.";
            isLastBuffer = true;
        }
        if (isLastBuffer && _myRepeat) {
            isLastBuffer = false;
            // Do an exact repeat by filling the rest of the buffer with data from
            // the beginning of the file.
            _myInFile.seekg(_myDataStart, std::ios::beg);
            myBytesToRead = myBytesToRead-myBytesRead;
            _myInFile.read(&(*_myFileSamples.begin()), myBytesToRead);
            myBytesRead = _myInFile.gcount();
            if (myBytesRead != myBytesToRead) {
                AC_WARNING << "Truncated wav file.";
                isLastBuffer = true;
            }
        }
        if (isLastBuffer && !_myRepeat) {
            // At the end of the file, we need to fill the rest of the 
            // buffer with silence.
            for (int i = myBytesRead; i<myBytesPerBuffer; i++) {
                _myFileSamples[i] = 0; 
            }
        }

        if (_mySampleSize == 16) {
            if (getNumChannels() == 1) {
                _myLeftBuffer->set16Bit ((asl::Signed16*)&(*_myFileSamples.begin()), Mono, getNumChannels());
            } else {
                _myLeftBuffer->set16Bit ((asl::Signed16*)&(*_myFileSamples.begin()), Left, getNumChannels());
                _myRightBuffer->set16Bit ((asl::Signed16*)&(*_myFileSamples.begin()), Right, getNumChannels());
            }
        } else {
            if (getNumChannels() == 1) {
                _myLeftBuffer->set24Bit (&(*_myFileSamples.begin()), Mono, getNumChannels());
            } else {
                _myLeftBuffer->set24Bit (&(*_myFileSamples.begin()), Left, getNumChannels());
                _myRightBuffer->set24Bit (&(*_myFileSamples.begin()), Right, getNumChannels());
            }
        }

        _myLeftOutput->put(*_myLeftBuffer);
        if (getNumChannels() == 2) {
            _myRightOutput->put(*_myRightBuffer);
        }

        _mySamplesPlayed += _myLeftBuffer->getNumSamples();
        //    AC_DB("WAVReader::process: " << _mySamplesPlayed);
        if (((_myState == STOPPING) && (_mySamplesPlayed-_myStopSample > FADE_DURATION)) ||
                isLastBuffer) 
        {
            _myState = STOPPED;
            AudioScheduler::get().sendStop(this);
            //        AC_DB("WAVReader::sendStop: " << getName()); 
        }
    } catch (const std::ios::failure & ex) {
        throw SoundException(std::string("WAVReader::process(): stream error :")+ex.what(),PLUS_FILE_LINE);
    }
}

void 
WAVReader::readWAVHeader (std::ifstream & theInFile) {
    theInFile.read(reinterpret_cast<char*>(&_myHeader), sizeof(WAVHeader));
    if (!theInFile) {
        throw SoundException("Couldn't read file header.",PLUS_FILE_LINE);
    }
    theInFile.seekg(_myHeader.len+20, std::ios::beg);
    char myChunkID[4]; 
    asl::Signed32 myChunkSize;
    theInFile.read(reinterpret_cast<char*>(&myChunkID), sizeof(char)*4);
    theInFile.read(reinterpret_cast<char*>(&myChunkSize), sizeof(asl::Signed32)*1);

    if (_myHeader.magic != *(asl::Signed32 *)RIFF_ID || 
        _myHeader.magic1 != *(asl::Signed32 *)WAVE_ID ||
        _myHeader.magic2 != *(asl::Signed32 *)FMT_ID) {
        throw SoundException ((_myFName+" isn't a .wav file").c_str(), 
                              PLUS_FILE_LINE);
    }

    // Skip over all non-data chunks.
    while (*(asl::Signed32*)myChunkID != *(asl::Signed32*)DATA_ID) {
        theInFile.seekg(myChunkSize, std::ios::cur);
        theInFile.read(reinterpret_cast<char*>(&myChunkID),sizeof(char)* 4);
        theInFile.read(reinterpret_cast<char*>(&myChunkSize),sizeof(asl::Signed32) * 1);
    }
    _myDataStart = theInFile.tellg();
    _myDataLen = myChunkSize;          
    _mySampleSize = _myHeader.size;
    if (_myHeader.rate != getSampleRate()) {
        throw SoundException ((_myFName+" has unsupported sample rate "+
                asl::as_string(_myHeader.rate)+". Should be "+
                asl::as_string(getSampleRate())).c_str(), 
                PLUS_FILE_LINE);
    }
    if (_myHeader.size != 16 && _myHeader.size != 24) {
        throw SoundException ((_myFName+" has unsupported header size.").c_str(), 
                              PLUS_FILE_LINE);
    }
    ASSURE(_myHeader.nBlockAlign == _myHeader.nchns*_myHeader.size/8);
    _myNumChannels = _myHeader.nchns;
//    dumpWAVHeader();
}


int
WAVReader::getCurSample () {
    try {
        return (_myInFile.tellg()-_myDataStart)/(_myNumChannels*_mySampleSize/8);
    } catch (const std::ios::failure & ex) {
        throw SoundException(std::string("WAVReader::getCurSample(): stream error :")+ex.what(),PLUS_FILE_LINE);
    } 
}

bool
WAVReader::setCurSample (std::ifstream::pos_type myPos) {
    try {
        std::ifstream::pos_type myFilePos = _myDataStart+myPos*_myNumChannels*_mySampleSize/8;
        if (myFilePos <= _myDataLen+_myDataStart) {
            _myInFile.seekg(myFilePos, std::ios::beg);
            return true;
        } else {
            // Seek beyond end of data.
            return false;
        }
    } catch (const std::ios::failure & ex) {
        throw SoundException(std::string("WAVReader::setCurSample(): stream error :")+ex.what(),PLUS_FILE_LINE);
    } 
}

} 

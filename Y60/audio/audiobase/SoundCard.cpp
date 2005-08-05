//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: SoundCard.cpp,v $
//
//     $Author: valentin $
//
//   $Revision: 1.10 $
//
//
// Description: 
//
//
//=============================================================================

#include "SoundCard.h"
#include "AudioBuffer.h"
#include "SoundException.h"

#include <asl/Logger.h>
#include <iostream>
#include <sstream>

using namespace std;  

namespace AudioBase {

    SoundCard::SoundCard () :
        _myLatency(0),
        _mySampleRate(48000),
        _myNumOutChannels(0),
        _myNumInChannels(0),
        _mySampleBits(16),
        _myIsOutOpen(false),
        _myIsInOpen(false),
        _myIsInitialized(false),
        _myAC3Context(0),
        _myAC3Codec(0)
    {
    }

    SoundCard::~SoundCard () {
#ifndef WIN32
        // TODO: Make this work under windows.
        closeOutput();
        closeInput();
#endif    
    }

    void SoundCard::init(const std::string & myDevName, const std::string & myCardName, 
                         int myMaxOutChannels, int myMaxInChannels)
    {
        _myDevName = myDevName;
        _myCardName = myCardName;
        _myMaxOutChannels = myMaxOutChannels;
        _myMaxInChannels = myMaxInChannels;
        _myIsInitialized = true;
    }

    void
    SoundCard::setParams(double theLatency, int theSampleRate,
                         int theSampleBits, unsigned theNumChannels)
    {
        if (theLatency != _myLatency || theSampleRate != _mySampleRate ||
            theSampleBits != _mySampleBits || theNumChannels != _myNumOutChannels) {
            if (isOutputOpen()) {
                AC_WARNING << "SoundCard::setParams: Output is open, closing";
                closeOutput();
            }
            if (isInputOpen()) {
                AC_WARNING << "SoundCard::setParams: Input is open, closing";
                closeInput();
            }
        }

        _myLatency = theLatency;
        _mySampleRate = theSampleRate;
        _mySampleBits = theSampleBits;
        _myNumOutChannels = theNumChannels;
        if (theNumChannels > _myMaxOutChannels) {
            stringstream s;
            s << "SoundCard::setParams called for " << theNumChannels << 
                    " channels but sound card only supports " << _myMaxOutChannels << ".";
            throw SoundException(s.str(), PLUS_FILE_LINE);
        }
    }

    void 
    SoundCard::openOutput() {
        if (!_myIsInitialized) {
            AC_WARNING << "SoundCard::openOutput called for uninitialized card. Ignoring.";
            return;
        }
        _myIsOutOpen = true;
    }
    
    void 
    SoundCard::openInput(int theNumChannels) {
        if (!_myIsInitialized) {
            AC_WARNING << "SoundCard::openInput called for uninitialized card. Ignoring";
            return;
        }
        if (theNumChannels > _myMaxInChannels) {
            AC_WARNING << "SoundCard::openInput called for " << theNumChannels << 
                    " channels. The sound card only supports " << _myMaxInChannels;
            return;
        }
        _myIsInOpen = true;
        _myNumInChannels = theNumChannels;
    }
    
    void 
    SoundCard::closeOutput() {
        _myIsOutOpen = false;
    }

    void 
    SoundCard::closeInput() {
        _myIsInOpen = false;
    }

    bool 
    SoundCard::isOutputOpen() const {
        return _myIsOutOpen;
    }

    bool 
    SoundCard::isInputOpen() const {
        return _myIsInOpen;
    }

    const string &
    SoundCard::getCardName() const {
        return _myCardName;
    }

    const string & 
    SoundCard::getDeviceName() const {
        return _myDevName;
    }

    int 
    SoundCard::getMaxOutChannels() const {
        return _myMaxOutChannels;
    }

    int 
    SoundCard::getMaxInChannels() const {
        return _myMaxInChannels;
    }

    int 
    SoundCard::getNumOutChannels() const {
        return _myNumOutChannels;
    }

    int 
    SoundCard::getNumInChannels() const {
        return _myNumInChannels;
    }

    int 
    SoundCard::getSampleRate() const {
        return _mySampleRate;
    }

    int
    SoundCard::getSampleBits() const {
        return _mySampleBits;
    }

    int
    SoundCard::getSampleSize() const {
        return _mySampleBits/8;
    }


    double 
    SoundCard::getLatency() const {
        return _myLatency;
    }

    void SoundCard::setLatency(double theLatency) {
        _myLatency = theLatency;
    }


    int SoundCard::initAC3encoder(int theSampleRate, int theBitrate, int theChannels)
    {
        //avcodec_init();

        /* register ac3 encoder */
        av_register_all();
        //register_avcodec(&ac3_encoder);

        /* find the ac3 encoder */
        _myAC3Codec = avcodec_find_encoder(CODEC_ID_AC3);

        if (!_myAC3Codec) {
            AC_WARNING << "avcodec_find_encoder failed.";
        }

        if (_myAC3Context)
        {
            AC_ERROR << "ac3_init_encoder called twice";
            return 0;
        }
        _myAC3Context = (AVCodecContext *) av_malloc (sizeof(AVCodecContext));
        memset(_myAC3Context, 0, sizeof(*_myAC3Context));
        _myAC3Context->sample_rate=theSampleRate;
        _myAC3Context->bit_rate=theBitrate;
        _myAC3Context->channels=theChannels;
        return avcodec_open(_myAC3Context, _myAC3Codec);
    }

    int SoundCard::stopAC3encoder(void)
    {
        if (_myAC3Context)
        {
            //MessageBox(NULL, "Stop Engine: ctx ok", "ac3encode", MB_OK);
            avcodec_close(_myAC3Context);
            av_free(_myAC3Context);
            _myAC3Context = NULL;
            return 0;
        }
        else
        {
            AC_ERROR << "Stop Engine: no Context";
            return 1;
        }

    }

    int SoundCard::encodeAC3frame(unsigned char *theOutputBuffer, int theOutputBufferSize, const asl::Signed16 *theSamples)
    {
        if (_myAC3Context) {
            return avcodec_encode_audio(_myAC3Context, theOutputBuffer, theOutputBufferSize, theSamples);
        } else {
            AC_ERROR << "Encode frame called without init!";
            return 0;
        }

    }

    const AVCodecContext * SoundCard::getAC3Context() const {
        return _myAC3Context;
    }


    ostream& operator << (ostream& strm, const SoundCard& theCard) {
        strm << theCard.asString() << endl;
        return strm;
    }

}

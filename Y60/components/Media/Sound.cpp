//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "Sound.h"

#include <asl/Auto.h>
#include <asl/Pump.h>

using namespace std;
using namespace asl;

namespace y60 {

Sound::Sound (string myURI, HWSampleSinkPtr mySampleSink, bool theLoop)
    : _myURI (myURI),
      _mySampleSink(mySampleSink),
      _mySamples(AVCODEC_MAX_AUDIO_FRAME_SIZE),
      _myFormatContext(0),
      _myStreamIndex(-1),
      _myDecodingComplete(false),
      _myIsLooping(theLoop),
      _myTargetBufferedTime(1.0),
      _myMaxUpdateTime(0.2)
{
    AC_DEBUG << "Sound::Sound";
    _myLockedSelf = SoundPtr(0);
    open();
}

Sound::Sound (const string & myURI, Ptr < ReadableStream > myStream,
        HWSampleSinkPtr mySampleSink, bool theLoo)
{
    AC_DEBUG << "Sound::Sound (" << _myURI << ")";
    _myLockedSelf = SoundPtr(0);
}

Sound::~Sound()
{
    AC_DEBUG << "Sound::~Sound (" << _myURI << ")";
    _myBufferCache.clear();
    close();
}

void Sound::setSelf(const SoundPtr& mySelf)
{
    _mySelf = mySelf;
    _myLockedSelf = SoundPtr(0);
}

void Sound::play ()
{
    AutoLocker<ThreadLock> myLocker(_myLock);
    AC_DEBUG << "Sound::play (" << _myURI << ")";
    if (!isOpen()) {
        open();
    }
    _myLockedSelf = _mySelf.lock();
    update(0.1);
    _mySampleSink->play();
}

void Sound::pause ()
{
    AutoLocker<ThreadLock> myLocker(_myLock);
    AC_DEBUG << "Sound::pause (" << _myURI << ")";
    _mySampleSink->pause();
    _myLockedSelf = SoundPtr(0);
}

void Sound::stop ()
{
    AutoLocker<ThreadLock> myLocker(_myLock);
    AC_DEBUG << "Sound::stop (" << _myURI << ")";
    _mySampleSink->stop();
    _myBufferCache.clear();
    close();
    _myLockedSelf = SoundPtr(0);
}

void Sound::setVolume (float theVolume)
{
    _mySampleSink->setVolume(theVolume);
}

void Sound::fadeToVolume (float theVolume, Time theTime)
{
    _mySampleSink->fadeToVolume(theVolume, float(theTime));
}

double Sound::getVolume() const {
    return _mySampleSink->getVolume();
}

std::string Sound::getName() const {
    return _myURI;
}

Time Sound::getDuration() const {
    AutoLocker<ThreadLock> myLocker(_myLock);
    if (_myFormatContext) {
        return (_myFormatContext->streams[_myStreamIndex]->duration/double(AV_TIME_BASE));
    } else {
        return 0;
    }
}

Time Sound::getCurrentTime() {
    return _mySampleSink->getCurrentTime();
}

void Sound::seek (Time thePosition)
{
    AutoLocker<ThreadLock> myLocker(_myLock);
    AC_DEBUG << "Sound::seek()";
    bool myIsPlaying = isPlaying();
    _mySampleSink->stop();
    _myBufferCache.clear();
    // Forget the old sample sink. It'll fade out and then destroy itself.
    _mySampleSink = Pump::get().createSampleSink(_myURI);
    if (!isOpen()) {
        open();
    }
#if (LIBAVCODEC_BUILD < 4738)
    int ret = av_seek_frame(_myFormatContext, -1, (long long)(thePosition*AV_TIME_BASE));
#else
    int ret = av_seek_frame(_myFormatContext, -1, (long long)(thePosition*AV_TIME_BASE), 
            AVSEEK_FLAG_BACKWARD);
#endif
    if (ret < 0) {
        AC_WARNING << "Unable to seek to timestamp=" << thePosition;
        return;
    }
    _mySampleSink->setCurrentTime(thePosition);
    _myLockedSelf = _mySelf.lock();
    if (myIsPlaying) {
        update(0.1);
        _mySampleSink->play();
    }
}

void Sound::seekRelative (Time theAmount)
{
    AutoLocker<ThreadLock> myLocker(_myLock);
}

Time Sound::getBufferedTime () const
{
    return _mySampleSink->getBufferedTime();
}

bool Sound::canSeek() const {
    return false;
}

bool Sound::isPlaying() const {
    return (_mySampleSink->getState() == HWSampleSink::RUNNING);
}

unsigned Sound::getNumUnderruns() const {
    return _mySampleSink->getNumUnderruns();
}

void Sound::update(double theTimeSlice) {
    AutoLocker<ThreadLock> myLocker(_myLock);

    double myBuffersFilledRatio = double(_mySampleSink->getBufferedTime())/
            _myTargetBufferedTime;
    double myTimeToBuffer = double(_mySampleSink->getBufferedTime())+
            myBuffersFilledRatio*theTimeSlice+
            (1-myBuffersFilledRatio)*_myMaxUpdateTime;
    if (_myDecodingComplete) {
        if (_myIsLooping) {
            if (isPlaying()) {
                while (double(_mySampleSink->getBufferedTime()) < myTimeToBuffer) {
                    // queueSamples pushes the buffer onto the list again :-).
                    queueSamples(_myBufferCache.front());
                    _myBufferCache.pop_front();
                }
            }
        } else {
            if (!isPlaying()) {
                _myDecodingComplete = false;
                _myLockedSelf = SoundPtr(0);
            }
        }
    }
    if (!_myDecodingComplete) {
        bool myEOF = false;
        if (isOpen()) {
            while (double(_mySampleSink->getBufferedTime()) < myTimeToBuffer && !myEOF) {
                myEOF = decode();
            }
            if (myEOF) {
                _myDecodingComplete = true;
                if (!_myIsLooping) {
                    _mySampleSink->stop(true);
//                    _myLockedSelf = SoundPtr(0);
                }
                close();
            }
        }
    }
}

void Sound::open() {
    AC_DEBUG << "Sound::open (" << _myURI << ")" << _myURI;

    // register all formats and codecs
    static bool avRegistered = false;
    if (!avRegistered) {
        AC_DEBUG << "Sound::open: " << LIBAVCODEC_IDENT << endl;
        //av_log_set_level(AV_LOG_ERROR);
        av_register_all();
        avRegistered = true;
    }

    int err;
    if ((err = av_open_input_file(&_myFormatContext, _myURI.c_str(), 0, 0, 0)) < 0) {
        throw SoundException(std::string("Unable to open input file, err=") + 
                asl::as_string(err) + ": " + _myURI, PLUS_FILE_LINE);
    }
    if ((err = av_find_stream_info(_myFormatContext)) < 0) {
        throw SoundException(std::string("Unable to find stream info, err=") + 
                asl::as_string(err) + ": " + _myURI, PLUS_FILE_LINE);
    }
    // find first audio stream
    _myStreamIndex = -1;
    for (unsigned int i = 0; i < _myFormatContext->nb_streams; ++i) {
        if (_myFormatContext->streams[i]->codec.codec_type == CODEC_TYPE_AUDIO) {
            _myStreamIndex = i;
            break;
        }
    }
    if (_myStreamIndex < 0) {
        throw SoundException(std::string("No audio stream found: ") + _myURI, 
                PLUS_FILE_LINE);
    }

    // open codec
    AVCodecContext * myCodecContext = &_myFormatContext->streams[_myStreamIndex]->codec;
    AVCodec * myCodec = avcodec_find_decoder(myCodecContext->codec_id);
    if (!myCodec) {
        throw SoundException(std::string("Unable to find decoder: ") + _myURI, PLUS_FILE_LINE);
    }
    if (avcodec_open(myCodecContext, myCodec) < 0 ) {
        throw SoundException(std::string("Unable to open codec: ") + _myURI, PLUS_FILE_LINE);
    }

    _mySampleRate = myCodecContext->sample_rate;
    _myNumChannels = myCodecContext->channels;
    AC_INFO << "Number of channels: " << _myNumChannels << endl;
    AC_INFO << "Sample rate: " << _mySampleRate << endl;
}

void Sound::close() {
    AutoLocker<ThreadLock> myLocker(_myLock);
    AC_DEBUG << "Sound::close() (" << _myURI << ")";

    if (_myFormatContext) {
        AVCodecContext * myCodecContext = &_myFormatContext->streams[_myStreamIndex]->codec;
        if (myCodecContext) {
            avcodec_close(myCodecContext);
        }
        av_close_input_file(_myFormatContext);
        _myFormatContext = 0;
        _myStreamIndex = -1;
    }
}

bool Sound::isOpen() const {
    return (_myFormatContext != 0);
}

bool Sound::decode() {
    ASSURE(_myFormatContext);
    AVPacket myPacket;

    AVCodecContext * myCodec = &(_myFormatContext->streams[_myStreamIndex]->codec);
    
    int err = av_read_frame(_myFormatContext, &myPacket);
    if (err < 0) {
        return true;
    }
    if (myPacket.stream_index == _myStreamIndex) {
        int myBytesDecoded = 0; // decompressed data in BYTES
        unsigned char* myData = myPacket.data;
        int myDataLen = myPacket.size;
        int myLen = 0;
        while (myDataLen > 0) {
            myLen = avcodec_decode_audio(myCodec, (int16_t*)_mySamples.begin(), 
                    &myBytesDecoded, myData, myDataLen);
            if (myLen > 0 && myBytesDecoded > 0) {
                AudioBufferPtr myBuffer = _mySampleSink->createBuffer(
                        myBytesDecoded/(getBytesPerSample(SF_S16)*_myNumChannels));
                myBuffer->convert(_mySamples.begin(), SF_S16, _mySampleRate, _myNumChannels);
                queueSamples(myBuffer);
                myData += myLen;
                myDataLen -= myLen;
            } else {
                AC_WARNING << "Unable to avcodec_decode_audio";
                break;
            }
        }
    }
    av_free_packet(&myPacket);
    return false;
}

void Sound::queueSamples(AudioBufferPtr theBuffer) {
    _mySampleSink->queueSamples(theBuffer);
    if (_myIsLooping) {
        _myBufferCache.push_back(theBuffer);
    }
}

}


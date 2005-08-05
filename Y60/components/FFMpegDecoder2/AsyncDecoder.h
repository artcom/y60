//============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $RCSfile: AsyncDecoder.h,v $
//
//   $Author: jens $
//
//   $Revision: 1.5 $
//
//=============================================================================

#ifndef _ac_video_AsyncDecoder_h_
#define _ac_video_AsyncDecoder_h_

#include <y60/MovieDecoderBase.h>

#include <asl/ThreadLock.h>
#include <asl/Logger.h>
#include <audio/AudioController.h>
#include <audio/BufferedSource.h>


namespace y60 {
    
    /**
     * @ingroup y60video
     * Abstract base class for asynchronous decoders for audio/video movies.
     */
    class AsyncDecoder : public MovieDecoderBase {
    public:
        AsyncDecoder() :
          _myReadEOF(false), _myAudioStartTime(0), _myMovieTime(0),
              _myAudioBufferedSource(0), _myPauseStartTime(0), _myState(IDLE)
          {}

          /**
           * computes the audio time for audio/video sync. If no audio is played,
           * returns theSystemTime.
           * @param theSystemTime playbacktime passed since start of playback
           * @return if the movie has audio, the audio-time that audio is currently 
           *         played back, else theSystemTime
           */
          double getMovieTime(double theSystemTime) {
              if (!hasAudio()) {
                  //AC_TRACE << "No Audio";
                  return MovieDecoderBase::getMovieTime(theSystemTime);
              } else {
                  if (_myAudioBufferedSource->isRunning()) {
                      double myAudioTime = AudioApp::AudioController::get().getCurrentTime() - _myAudioStartTime;
                      double myMovieTime = myAudioTime + getAVDelay();
                      _myMovieTime = myMovieTime;
                  } 
                  return _myMovieTime;
              }
          }

          /**
           * Pauses the current playback. Audio is switched to nullsource
           */
          virtual void pauseMovie() {
              AC_INFO << "pauseMovie";
              if (_myAudioBufferedSource) {
                  _myAudioBufferedSource->setRunning(false);
                  AudioApp::AudioController & myAudioController = AudioApp::AudioController::get();
                  _myPauseStartTime = myAudioController.getCurrentTime();
              } else {
                  _myPauseStartTime = 0.0;
              }
              _myState = PAUSE;
              MovieDecoderBase::pauseMovie();
          }

          /**
           * Resumes from pause
           */
          void resumeMovie() {
              AC_DEBUG << "resumeMovie";
              double myPauseTime = 0.0;
              if (_myAudioBufferedSource) {
                  AudioApp::AudioController & myAudioController = AudioApp::AudioController::get();
                  myPauseTime = myAudioController.getCurrentTime() - _myPauseStartTime;
                  _myAudioBufferedSource->setRunning(true);
                  //AC_TRACE << "resume mpeg video";
              } else {
                  //AC_TRACE << "resume audio free mpeg video";
                  myPauseTime = _myPauseStartTime;
              }
              _myAudioStartTime += myPauseTime;
              _myPauseStartTime = -1;
          }

          /**
           * Stops the current playback. Future calls to play will start from the beginning of 
           * the movie as if it was 
           */
          virtual void stopMovie() {
              MovieDecoderBase::stopMovie();
              _myAudioStartTime = _myPauseStartTime = _myMovieTime = 0;
          }

          /**
           * Lockable interface.
           */
          void lock() {
              _myLock.lock();
          }
          void unlock() {
              _myLock.unlock();
          }

    protected:
        AudioBase::BufferedSource * _myAudioBufferedSource;
        void initBufferedSource(unsigned theNumChannels, unsigned theSampleRate, unsigned theSampleBits) {
            AudioApp::AudioController & myAudioController = AudioApp::AudioController::get();
            std::string myURL = getMovie()->get<ImageSourceTag>();
            if (theSampleBits != 16) {
                AC_WARNING << "Movie '" << myURL << "' contains " << theSampleBits
                    << " bit audio stream. Only 16 bit supported. Playing movie without sound.";
            } else {
                if (!myAudioController.isRunning()) {
                    myAudioController.init(asl::maximum(theSampleRate, (unsigned) 44100));
                }

                std::string myId = myAudioController.createReader(
                    myURL, "Mixer", theSampleRate, theNumChannels);
                _myAudioBufferedSource = dynamic_cast<AudioBase::BufferedSource *>(myAudioController.getFileReaderFromID(myId));
                _myAudioBufferedSource->setVolume(getMovie()->get<VolumeTag>());
                _myAudioBufferedSource->setSampleBits(theSampleBits);
                AC_INFO << "Audio: channels=" << theNumChannels << ", samplerate=" << theSampleRate << ", samplebits=" << theSampleBits;
            }

        }

        double          _myMovieTime;
        double          _myAudioStartTime;
        double          _myPauseStartTime;
        bool            _myReadEOF;
        asl::ThreadLock _myLock;

        enum DecoderState {
            IDLE,
            RUN,
            PAUSE,
            STOP
        };
        DecoderState      _myState;
    };
}
#endif // _ac_video_AsyncDecoder_h_

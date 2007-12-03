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

#include <asl/HWSampleSink.h>

namespace y60 {
    
    /**
     * @ingroup y60video
     * Abstract base class for asynchronous decoders for audio/video movies.
     */
    class AsyncDecoder : public MovieDecoderBase {
        public:
            AsyncDecoder() :
                _myState(STOP),
                _myLastAudioTime(0),
                _myCachingFlag(false)
            {}

            /**
             * @return true if the file currently decoded has audio
             * derived classes should set _myAudioSink according to media
             */
            virtual bool hasAudio() const {
                return _myAudioSink != 0;
            }

            /**
             * computes the audio time for audio/video sync. If no audio is played,
             * returns theSystemTime.
             * @param theSystemTime playbacktime passed since start of playback
             * @return if the movie has audio, the audio-time that audio is currently 
             *         played back, else theSystemTime
             */
double ourLastFrameTimeStamp;
             
            double getMovieTime(double theSystemTime) {
                if (!hasAudio() || !getDecodeAudioFlag()) {
                    AC_DEBUG << "No Audio returning " << MovieDecoderBase::getMovieTime(theSystemTime);
                    return MovieDecoderBase::getMovieTime(theSystemTime);
                } else {
                    if (_myAudioSink->getState() == asl::HWSampleSink::STOPPED &&
                            getState() != STOP)
                    {
                        // We only get here if video is running way behind audio, resulting in
                        // the AudioSink being stopped and the video still running. In that
                        // case, we return a time just after the last audio time that was 
                        // reported. This is just after the last frame and causes everything
                        // to be played back.
                        AC_DEBUG << "AsyncDecoder::getMovieTime: audio eof, returning "
                                << _myLastAudioTime+0.5;
                        return _myLastAudioTime+0.5;
                    } else {
                        AC_DEBUG << " returning audio time " << _myAudioSink->getCurrentTime()
                                <<"video time: "<<MovieDecoderBase::getMovieTime(theSystemTime);
                        _myLastAudioTime = _myAudioSink->getCurrentTime();
                        // audio is not running yet, maybe cause we are buffering, so do not show any video frames
                        if (_myAudioSink->getState() == asl::HWSampleSink::STOPPED || _myCachingFlag) {
                            return 0;
                        } else {
                            //AC_PRINT << " ASyncDecoder::getMovieTime------\nrendertimediff : " << (MovieDecoderBase::getMovieTime(theSystemTime) - ourLastFrameTimeStamp);
                            ourLastFrameTimeStamp = MovieDecoderBase::getMovieTime(theSystemTime);
                            //return _myAudioSink->getCurrentTime();
                            return _myAudioSink->getPumpTime();                            
                            //return MovieDecoderBase::getMovieTime(theSystemTime);
                        }
                    }
                }
            }

            /**
             * Pauses the current playback. Audio is paused as well.
             */
            virtual void pauseMovie(bool thePauseAudioFlag = true) {
                AC_INFO << "AsyncDecoder::pauseMovie";
                if (thePauseAudioFlag && _myAudioSink) {            
                    _myAudioSink->pause();
                }
                setState(PAUSE);
                MovieDecoderBase::pauseMovie();
            }

            void startMovie(double theStartTime, bool theStartAudioFlag = true) {
                AC_DEBUG << "AsyncDecoder::startMovie";
                MovieDecoderBase::startMovie(theStartTime);
                if (theStartAudioFlag && _myAudioSink) {            
                    _myAudioSink->play();
                }
            }
            /**
             * Resumes from pause
             */
            void resumeMovie(double theStartTime, bool theResumeAudioFlag = true) {
                AC_DEBUG << "AsyncDecoder::resumeMovie";
                MovieDecoderBase::resumeMovie(theStartTime);
                if (theResumeAudioFlag && _myAudioSink) {            
                    _myAudioSink->play();
                }
            }

            /**
             * Stops the current playback. Future calls to play will start from the beginning of 
             * the movie as if it was 
             */
            virtual void stopMovie(bool theStopAudioFlag = true) {
                AC_DEBUG << "AsyncDecoder::stopMovie";
                MovieDecoderBase::stopMovie();
                if (theStopAudioFlag && _myAudioSink) {            
                    _myAudioSink->stop();
                } 
            }

            enum DecoderState {
                RUN,
                PAUSE,
                STOP
            };
            
            static std::string getStateString(DecoderState theState) {
                switch (theState) {
                    case RUN:
                        return "RUN";
                    case PAUSE:
                        return "PAUSE";
                    case STOP:
                        return "STOP";
                    default:
                        return "Unknown state";
                }
            }
            
            DecoderState getState() const {
                return _myState;
            }

            void setState(DecoderState theState) {
                AC_DEBUG << "AsyncDecoder::setState State change: " << getStateString(_myState) << " --> " 
                        << getStateString(theState);
                _myState = theState;
            }
            
        protected:
            asl::HWSampleSinkPtr _myAudioSink;
            bool          _myCachingFlag;

        private:
            DecoderState _myState;
            double _myLastAudioTime;

    };
}
#endif // _ac_video_AsyncDecoder_h_

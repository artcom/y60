//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or dupli:mcated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: FileBasedMovie.cpp,v $
//
//     $Author: valentin $
//
//   $Revision: 1.2 $
//
//
// Description:
//
// (CVS log at the bottom of this file)
//
//
//=============================================================================


#include "MovieBase.h"
#include "FileBasedMovie.h"

#include <fcntl.h>
//#include <unistd.h>

#include <asl/math/numeric_functions.h>
#include <asl/base/string_functions.h>
#include <asl/base/Logger.h>
#include <GL/gl.h>
#include <asl/base/Time.h>

#define SHOW_DATE
#define DEBUG_LEVEL 1

using namespace std;  // automatically added!
using namespace asl;



namespace video {


using namespace TextureCompression;

    FileBasedMovie::FileBasedMovie(const char *fileName, bool showStatistic): 
        MovieBase(showStatistic), _myFileName(fileName), 
        _myCurrentDataBufferIdx(0),
        _myCurrentFrame(0),
        _lastTime(0),
        _movieStarted(false),
        _numDataBuffers(2),
        _myFullFrameDataSize(0),
        _allowDuplicateRequests(true),
        _selectedBufferCounter(0),
        _myAsyncLoadStrategy(STEADY),
        _myPredictionStrategy(CONST_DELTA),
        _myPredictionDelta(1),
        _myPredictionDeltaSignChanged(false)
    {
        for (int i=0; i< 4; i++) {
            _myDataBuffer[i] = 0;
        }
        _myDataType = OPENGL_COMPRESSED;

    }

   FileBasedMovie::~FileBasedMovie() {
        
        for (int i=0; i<_numDataBuffers; i++) {
            if (_myDataBuffer[i]) {
                free(_myDataBuffer[i]);
                _myDataBuffer[i] = 0;
            }
        }
    }

    void
    FileBasedMovie::reset() {
        MovieBase::reset();
        _myCurrentFrame = 0;
    }

    void 
    FileBasedMovie::setPredictionStrategy(PREDICTION_STRATEGY theStrategy, long long theDelta) {
        _myPredictionStrategy = theStrategy;
        _myPredictionDeltaSignChanged = false;
        if ((theDelta != _myPredictionDelta) && (theDelta !=0)) {
            _myPredictionDeltaSignChanged = true;
        }
        _myPredictionDelta = theDelta;
    }

    
    void
    FileBasedMovie::allowDuplicateRequests(bool doAllow) {
        _allowDuplicateRequests = doAllow;
    }
    
    void    
    FileBasedMovie::getTextureInfo(unsigned int & theTextureWidth, 
                                   unsigned int & theTextureHeight, 
                                   unsigned int & theTextureNumBytes) const
    {
        theTextureWidth = _FileHeader.Width;
        theTextureHeight = _FileHeader.Height;
        theTextureNumBytes = _FileHeader.FrameDataSize;
    }
                               
    bool 
    FileBasedMovie::init() {      
        _myCurrentFrame = 0;
        //_myCompressedMovieBlock = asl::Ptr<asl::ReadableFile>(new asl::ReadableFile(_myFileName));
        _myCompressedMovieBlock = asl::Ptr<std::ifstream>(new std::ifstream());
        _myCompressedMovieBlock->open (_myFileName.c_str(), ios::binary );
        _myCompressedMovieBlock->seekg (0, ios::beg);

        int theIncreaseData = 0;
        if(_myCompressedMovieBlock) {
            if ( readOGLTHeader(_myCompressedMovieBlock, _FileHeader, _showStatistic) ) {
                
                _myWidthAspect  = _FileHeader.WidthAspect;
                _myHeightAspect = _FileHeader.HeightAspect;
                _myGLCompressionFormat = getGLFormat((COMPRESSION_FORMAT)
                                                     _FileHeader.OpenGLCompression);
                _myFPS = _FileHeader.FramesPerSecond;
                _FrameCount = _FileHeader.FrameCount;
                _myFrameWidth = _FileHeader.Width;
                _myFrameHeight = _FileHeader.Height;
                if (!asl::powerOfTwo(_myFrameWidth) ) {
                    AC_ERROR << "FileBasedMovie::init() "
                            << "at least one side must be power of two."; 
                    return false;
                }
                _myComponents = _FileHeader.Components;
                // check for power of two
                if (!asl::powerOfTwo(_myFrameHeight) ) {
                    int newHeight = calcTextureSize (_myFrameHeight);
                    
                    theIncreaseData = ( (_myFrameWidth * newHeight ) -  
                          (_myFrameWidth *  _myFrameHeight ) ) * 4 /
                          getCompressionRatio(getFormatFromGLFormat(_myGLCompressionFormat));

                    _myHeightStretch = float(_myFrameHeight) / float(newHeight);
                    _myFrameHeight = newHeight; 
                    
                }
                _myFrameDataSize = _FileHeader.FrameDataSize ;
                _myTextureDataSize = _myFrameDataSize + theIncreaseData;

                assert(_numDataBuffers > 0);
                assert(_numDataBuffers <= 4);
                _myFullFrameDataSize = (_myFrameDataSize + theIncreaseData )/sizeof(GLubyte);
                for (int i=0; i< _numDataBuffers; i++) {
                    _myDataBuffer[i] = (unsigned char*) realloc(_myDataBuffer[i], 
                            _myFullFrameDataSize);
                    assert(_myDataBuffer[i]);
                }
                _myCurrentFrameData = _myDataBuffer[0];

                _myRotation = _FileHeader.Orientation;
                
                return true;
            }
        }
        AC_ERROR << "FileBasedMovie::init() "
                << "couldn't open " << _myFileName;
        return false;
    }
    
    const Time 
    FileBasedMovie::getMovieDuration() {
        return ( double(_FrameCount/_myFPS) );
    }

    bool
    FileBasedMovie::updateToFrame(unsigned int myAbsoluteFrame) {

        if ((myAbsoluteFrame != _myAbsoluteFrameLast) || 
            (_myPredictionDeltaSignChanged)) {

            _myPredictionDeltaSignChanged = false;

            // blank frame
            if (myAbsoluteFrame == ULONG_MAX) {
                assert(_myDataBuffer[_myCurrentDataBufferIdx]);
                memset(_myDataBuffer[_myCurrentDataBufferIdx], 0, _myFrameDataSize);
                _myAbsoluteFrameLast = myAbsoluteFrame;
                return true;
            }

            unsigned int myFrame = getClampedFrame(myAbsoluteFrame);
            unsigned int myAbsoluteNextFrame = getPredictedFrame (myAbsoluteFrame);
            unsigned int myNextFrame = getClampedFrame (myAbsoluteNextFrame);
//cout << "Frame " << myNextFrame << "/" << _FrameCount << endl;
            // check loop
            _myCurrentLoop = myAbsoluteFrame / _FrameCount;
            if (!_repeat) {
                if (_myCurrentLoop != 0) {
                    AC_WARNING  <<"FileBasedMovie::updateToFrame() - "
                            <<"movie " << _myFileName.c_str() 
                            << " (frame #" << myFrame << ")"
                            << " has looped " << _myCurrentLoop << "x !";
                } else if (myNextFrame != myAbsoluteNextFrame) {
                    AC_DEBUG <<"FileBasedMovie::updateToFrame() - "
                            <<"movie " << _myFileName.c_str() << " is looping."    
                            <<" (nextframe #"<<myNextFrame 
                            <<", absolute next frame #" << myAbsoluteNextFrame
                            <<", frame count "<<_FrameCount
                            <<", current loop "<<_myCurrentLoop
                            <<", repeat " <<_repeat << ")";
                }
            }

            AC_DEBUG << "FileBasedMovie::updateToFrame()";
            AC_DEBUG << "    last absolute frame #" <<_myAbsoluteFrameLast
                    << ", absolute frame #"<<myAbsoluteFrame
                    << ", frame count " <<_FrameCount;
            AC_DEBUG << "    current frame #" << _myCurrentFrame 
                    << ", next frame #"<<myNextFrame;

            // synchronous loading

            Time startTime;

            long long frameStartPosition = (long long)(sizeof(OGLT_Header))
                                           + (long long)(myFrame) * _myFrameDataSize;

            void * myDataPtr = (void*)_myDataBuffer[_myCurrentDataBufferIdx];
            _myCompressedMovieBlock->read((char*)myDataPtr, _myFrameDataSize);
            long long myPosition = _myCompressedMovieBlock->tellg();
            //cout << "myPosition : " << myPosition << " / " << frameStartPosition << " frame: " << _myCurrentFrame << endl;
            //_myCompressedMovieBlock->read((char*)myDataPtr, _myFrameDataSize);
            //_myCompressedMovieBlock->readBytes(myDataPtr, _myFrameDataSize, frameStartPosition);
            _myCurrentFrame = myFrame;

            _myAbsoluteFrameLast = myAbsoluteFrame;
            return true;
        } else {
                AC_DEBUG << "FileBasedMovie::updateToFrame(): "
                        << "already requested frame #" << myAbsoluteFrame;
                return false;
            }
        }

    unsigned int 
    FileBasedMovie::getHeaderOffset() {
        return sizeof(OGLT_Header);
    }
    
    unsigned int 
    FileBasedMovie::getNumBytesPerFrame() {
        return _myFrameDataSize;
    }
 
    void
    FileBasedMovie::updateToFrameAsync(unsigned int frame) {

    }
    
    bool
    FileBasedMovie::waitForFrameAsync(unsigned int frame) {
        return true;
    }

    
    bool
    FileBasedMovie::updateToTime(double runningTime) {
        AC_DEBUG << "------------------------------------------------------";
        AC_DEBUG << "FileBasedMovie::updateToTime(" << runningTime << ")";
        unsigned int myAbsoluteFrame = _myCurrentFrame; //(unsigned long)
                                        //(round(runningTime * _myFPS));
        bool success = (updateToFrame (myAbsoluteFrame));        
        _myCurrentFrame++;
        return success;
    }

    unsigned int 
        FileBasedMovie::getClampedFrame (unsigned int currentFrame) {
            if (currentFrame < 0) {
                currentFrame = _FrameCount + (currentFrame % _FrameCount);    
            } else if (currentFrame >= _FrameCount){ 
                currentFrame = currentFrame % _FrameCount; 
                _myCompressedMovieBlock->seekg (sizeof(_FileHeader), ios::beg);
            }
            return currentFrame;
        }


    unsigned int 
    FileBasedMovie::getPredictedFrame (unsigned int currentFrame) {
        // prediction for preloading

        int frameDelta;            

        switch (_myPredictionStrategy) {
            case CONST_DELTA:
                frameDelta = static_cast<int>(_myPredictionDelta);
                break;
            case FRAME_DIFFERENCE:
                frameDelta = currentFrame - _myCurrentFrame;
                break;
            default:
                frameDelta = 1;
                break;
        }

        return currentFrame + frameDelta;
    }

    void    
    FileBasedMovie::setupRawStriping(unsigned int numStripes, const StripeInfo  stripeInfo[4]){

        assert(numStripes > 0);
        assert(numStripes <= 4);

        _numStripes = numStripes;
        _numDataBuffers = numStripes;
        
        cerr << "FileBasedMovie::setupRawStriping: num stripes= " << numStripes << endl;
        
        for (int i=0; i< _numStripes; i++) {
            _stripeInfo[i] = stripeInfo[i];
        
            cerr << "   rawdev" << i << ": " << _stripeInfo[i]._deviceName << endl;
         }
    }


} // namespace end

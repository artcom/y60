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


#if !defined(_ac_video_FileBasedVideo_h_)
#define _ac_video_FileBasedVideo_h_

#include "CompressionType.h"
#include <string>
#include <vector>

#include <asl/Base/Time.h>
#include <asl/base/MappedBlock.h>
#include "MovieBase.h"

namespace video {

class AsyncLoadThread;

class FileBasedMovie : public MovieBase {
    public:
       
        FileBasedMovie(const char* fileName, bool showStatistic);
        virtual ~FileBasedMovie();
        virtual bool init();
        virtual bool updateToTime(double runningTime);
        virtual bool updateToFrame(unsigned long frame);
        virtual void updateToFrameAsync(unsigned long frame);
        virtual bool waitForFrameAsync(unsigned long frame);
        
        virtual void reset();
        
        void         allowDuplicateRequests(bool doAllow);
        
        virtual const asl::Time getMovieDuration();

        virtual void    getTextureInfo(unsigned int & theTextureWidth, 
                               unsigned int & theTextureHeight, 
                               unsigned int & theTextureNumBytes) const;

        /*FD_t     getFileID() const {
            return _myCompressedMovieFile;
        }*/
        unsigned long getHeaderOffset();
        unsigned long getNumBytesPerFrame();
       
        virtual const std::string & getFileName() const {
            return _myFileName;
        }
        
        struct StripeInfo {
            StripeInfo() : _startOffset(0) {}
            StripeInfo(const StripeInfo & other) :
                _deviceName(other._deviceName),
                _startOffset(other._startOffset)
            {}
            StripeInfo & operator=(const StripeInfo & other) {
                _deviceName = other._deviceName;
                _startOffset = other._startOffset;
                return *this;
            }
            
            std::string _deviceName;
            unsigned long _startOffset;
        };
        
        void    setupRawStriping(unsigned int numStripes, const StripeInfo stripeInfo[4]);
       
        

        enum LOAD_STRATEGY {
            STEADY,  // No blocking wait, but not garanteed, that the requested frame will be shown
            PRECISE  // The requested frame will always be displayed
        };
        
        enum PREDICTION_STRATEGY {
            CONST_DELTA,       // Use a constant value for predicting the next frame
            FRAME_DIFFERENCE   // Use the difference between last and current frame to predict next frame
        };
         
        void setPredictionStrategy(PREDICTION_STRATEGY theStrategy, 
                                long long theDelta = 0);
        unsigned long getClampedFrame (unsigned long currentFrame);
        unsigned long getPredictedFrame (unsigned long currentFrame);

        void setStatistic (bool enable);

    protected:
        std::string     _myFileName;
        int             _myCurrentDataBufferIdx;
        unsigned char * _myDataBuffer[4];
        int             _numDataBuffers;
        unsigned int    _myFullFrameDataSize;

        //asl::Ptr<asl::ReadableFile> _myCompressedMovieBlock;
        asl::Ptr<std::ifstream>  _myCompressedMovieBlock;
        //asl::Ptr<asl::ReadableBlockHandle> _myCompressedMovieBlock;
        TextureCompression::OGLT_Header _FileHeader;
        AsyncLoadThread *               _loadThread[4];
        bool                            _allowDuplicateRequests;
        int                             _selectedBufferCounter;
        LOAD_STRATEGY                   _myAsyncLoadStrategy;
        PREDICTION_STRATEGY             _myPredictionStrategy;
        long long                       _myPredictionDelta;
        bool                            _myPredictionDeltaSignChanged;
        
        double          _lastTime;
        bool            _movieStarted;
        int             _numStripes;
        StripeInfo      _stripeInfo[4];
        unsigned long long       _myCurrentFrame;
};

} // namespace end

#endif

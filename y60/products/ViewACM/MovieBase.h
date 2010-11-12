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


#if !defined(_ac_video_MovieBase_h_)
#define _ac_video_MovieBase_h_

#include "CompressionType.h"
#include "Image.h"

#include <asl/Base/Time.h>

#include <string>
#include <vector>

namespace video {



class MovieBase : public Image {
    //friend class MovieScreen;
    //friend class FileBasedMovieTest;
    public:

        MovieBase(bool showStatistic): _myAbsoluteFrameLast(ULONG_MAX), _myCurrentFrame(ULONG_MAX), _myFPS(25.0),
                     _myWidthAspect(640), _myHeightAspect(480), _FrameCount(0),   
                      _myLastSecond(0), 
                      _myStatisticFPS(0), 
                      _showStatistic(showStatistic), 
                      _myCurrentLoop(0), 
                      _repeat(true) {}
        virtual ~MovieBase() {}
        virtual bool init() = 0;
        virtual bool updateToTime(double runningTime)   { return false; }
        virtual bool updateToFrame(unsigned long frame) { return false; }
        virtual void updateToFrameAsync(unsigned long frame) { }
        virtual bool waitForFrameAsync(unsigned long frame)  { return false; }
                           
        virtual bool updateToStream()                   { return false; }
        virtual void reset() { _myAbsoluteFrameLast = ULONG_MAX; _myCurrentFrame = ULONG_MAX; _myCurrentLoop = 0; }
        // virtual bool isLooping()                        { return _myLoopingStatus;}
        virtual bool hasLooped()                        { return _myCurrentLoop != 0;}
        virtual void setRepeat(bool repeat)             { _repeat = repeat;}
        virtual bool isRepeatOn()                       { return _repeat;}
        virtual const asl::Time getMovieDuration() { return 0.0; }

        virtual bool    supportsAsyncLoad() const { return false; }
        virtual void    enableAsyncLoad(bool enableIt=true) {}

        // XP interface

        int   getHeightAspect()                  { return _myHeightAspect; }
        int   getWidthAspect()                   { return _myWidthAspect;  }
        float getFPS()                           { return _myFPS; }
        void  setFPS(float fps)                  { _myFPS = fps;  }
        unsigned long getFrameCount()            { return _FrameCount;}
        double getTotalRunningTime()             { return _FrameCount / _myFPS; }
        unsigned long getCurrentFrame()          { return _myCurrentFrame;}
        void  setStatistic(bool showStatistic)   { _showStatistic = showStatistic;}
        bool  getStatistic()                     { return _showStatistic;}
     protected:

        unsigned long  _myAbsoluteFrameLast;
        unsigned long  _myCurrentFrame;
        int            _myWidthAspect;
        int            _myHeightAspect;
        float          _myFPS;
        unsigned long  _FrameCount;
        int            _myCurrentLoop;
        // the isDone() check is almost imposible to compute precise
        // bool           _myLoopingStatus;     
        bool           _repeat;

        // some statistic
        bool           _showStatistic;
        long           _myLastSecond;
        long           _myStatisticFPS;
};

} // namespace end

#endif

//=============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: FrameRateLimiter.js,v $
//   $Author: christian $
//   $Revision: 1.3 $
//   $Date: 2004/11/10 18:37:33 $
//
//=============================================================================

function FrameRateLimiter(theFrameRate) {
    var _myTargetFrameTime   = 1 / theFrameRate;
    var _myLastPlayTime      = 0;
    var _myStatisticsFlag    = false;
    var _myMissedFrames      = 0;
    var _myLastOverTime      = 0;

    this.onIdle = function(theTime) {
        var mySleepTime = 0;
        if (_myLastPlayTime) {
            var myCurrentFrameTime = theTime - _myLastPlayTime;
            if (myCurrentFrameTime < _myTargetFrameTime) {
                mySleepTime = _myTargetFrameTime - myCurrentFrameTime;
                msleep(mySleepTime * 1000);
            } else if (theTime > 1) {
                _myMissedFrames++;
                _myLastOverTime = myCurrentFrameTime - _myTargetFrameTime;
            }
        }

        _myLastPlayTime = theTime + mySleepTime;

        if (_myStatisticsFlag) {
            window.renderText([0.04, 0.06], "Missed frames: " + _myMissedFrames +
                    "  -  Last overtime: " + _myLastOverTime.toFixed(2), "SyntaxBold18");
        }
    }

    this.setStatistics = function(theFlag) {
        _myStatisticsFlag = theFlag;
    }

    this.getStatistics = function() {
        return _myStatisticsFlag;
    }
}

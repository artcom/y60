//=============================================================================
// Copyright (C) 2004-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

/*
 * You may want to consider using 'window.swapInterval' instead as it's
 * done in hardware and more precise than using the system timer...
 */

function FrameRateLimiter(theFrameRate) {
    var _myTargetFrameTime   = 1 / theFrameRate;
    var _myLastPlayTime      = 0;
    var _myStatisticsFlag    = false;
    var _myMissedFrames      = 0;
    var _myLastOverTime      = 0;

    this.onFrame = function(theTime) {
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
            window.renderText([24, 24], "Missed frames: " + _myMissedFrames +
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

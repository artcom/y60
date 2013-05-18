/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

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

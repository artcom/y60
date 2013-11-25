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

use("Y60JSSL.js");

function VideoRecorder(theFramesPerSecond, theDirectory, theRenderWindow) {

    var Public = this;

    //////////////////////////////////////////////////////////////////////
    // Public
    //////////////////////////////////////////////////////////////////////
    

    Public.__defineGetter__("enabled", function() {
        return _myEnabledFlag;
    });

    Public.__defineSetter__("enabled" , function(theFlag) {
        if (theFlag != _myEnabledFlag) {
            if (theFlag) {
                setup();
                _myFixedFrameTime = window.fixedFrameTime;
                window.fixedFrameTime = 1 / _myFramesPerSecond;
                print("Video Recorder enabled for directory: " + _myDirectory);
            } else {
                window.fixedFrameTime = _myFixedFrameTime;
                print("Video Recorder disabled");
            }
            _myEnabledFlag = theFlag;
        }
    });

    Public.onFrame = function(theRenderArea) {
        if (_myEnabledFlag) {
            var myFileName = _myDirectory + "/frame";
            myFileName += padStringFront(_myFrameCount++, "0", 5);
            myFileName += ".jpg";
            if (theRenderArea) {
                theRenderArea.renderToCanvas(true);
            } else {
                window.saveBuffer(myFileName, 3);
            }
            Logger.info("saving frame: " + myFileName);
        }
    };

    //////////////////////////////////////////////////////////////////////
    // Private
    //////////////////////////////////////////////////////////////////////

    function setup() {
        var myCounter = 0;
        var myDirectoryName = (theDirectory ? theDirectory : "frames");
        _myDirectory = myDirectoryName;
        do {
            _myDirectory = myDirectoryName + "_" + myCounter++;
            if (!fileExists(_myDirectory)) {
                createDirectory(_myDirectory);
                break;
            }
        } while (true);

        _myFrameCount = 0;
    }

    var _myFrameCount      = 0;
    var _myEnabledFlag     = false;
    var _myFramesPerSecond = theFramesPerSecond;
    var _myFixedFrameTime  = window.fixedFrameTime;
    var _myDirectory       = null;
}


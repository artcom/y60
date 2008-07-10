//=============================================================================
// Copyright (C) 2004-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("Y60JSSL.js");

function VideoRecorder(theFramesPerSecond, theDirectory, theRenderWindow) {

    var Public = this;

    //////////////////////////////////////////////////////////////////////
    // Public
    //////////////////////////////////////////////////////////////////////

    Public.enabled getter = function() {
        return _myEnabledFlag;
    }

    Public.enabled setter = function(theFlag) {
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
    }

    Public.onFrame = function() {
        if (_myEnabledFlag) {
            var myFileName = _myDirectory + "/frame";
            myFileName += padStringFront(_myFrameCount++, "0", 5);
            myFileName += ".png";
            if (theRenderArea) {
                theRenderArea.renderToCanvas(true);
            } else {
                window.saveBuffer(myFileName);
            }
            print(myFileName);
        }
    }

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

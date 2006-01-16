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

function VideoRecorder(theFramesPerSecond, theDirectory, thePrefix) {
    this.Constructor(this, theFramesPerSecond, theDirectory, thePrefix)
}

VideoRecorder.prototype.Constructor = function(self, theFramesPerSecond, theDirectory, thePrefix)
{
    var _myEnabled       = true;
    var _myFrameCount    = 0;
    var _myNextFrameTime = 0.0;
    var _myFrameDelta    = (theFramesPerSecond ? (1.0 / theFramesPerSecond) : 0.0);

    var _myDirectory     = (theDirectory ? theDirectory : "frames");
    var _myPrefix        = (thePrefix ? thePrefix : "frame");

    self.setEnable = function(theEnableFlag) {
        _myEnabled = theEnableFlag;
    }

    self.getEnable = function() {
        return _myEnabled;
    }

    self.onFrame = function(theTime) {
        if (_myEnabled && theTime >= _myNextFrameTime) {
            var myFileName = _myDirectory + "/" + _myPrefix;
            myFileName += padStringFront(_myFrameCount++, "0", 5);
            myFileName += ".png";
            window.saveBuffer(myFileName);
            _myNextFrameTime = theTime + _myFrameDelta;
        }
    }

    function setup() {
        if (!fileExists(_myDirectory)) {
            Logger.warning("Creating directory '" + _myDirectory + "'");
            createDirectory(_myDirectory);
        }
    }

    setup();
}

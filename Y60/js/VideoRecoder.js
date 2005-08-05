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
//   $RCSfile: VideoRecoder.js,v $
//   $Author: valentin $
//   $Revision: 1.2 $
//   $Date: 2005/03/08 13:48:38 $
//
//=============================================================================

function VideoRecoder(theDirectory, theFrameRate) {
    this.Constructor(this, theDirectory, theFrameRate)
}

VideoRecoder.prototype.Constructor = function(obj, theDirectory, theFrameRate) {
        
    var _myDirectory = theDirectory;
    var _myFrameTime = 1 / theFrameRate;
    var _myEnabled   = false;
    var _myTime      = 0;
    var _myScreenShotCounter = 0;
    
    obj.onIdle = function(theTime) {                     
        if (_myEnabled) {
            _myTime += _myFrameTime;
            
            var myFileName = _myDirectory + "/";
            if (_myScreenShotCounter < 10) {
                myFileName += "000";
            } else if (_myScreenShotCounter < 100) {
                myFileName += "00";
            } else if (_myScreenShotCounter < 1000) {
                myFileName += "0";
            }
            
            myFileName += _myScreenShotCounter++ + ".png";
                        
            window.saveBuffer(FRAMEBUFFER, myFileName);        
        } else {
            _myTime = theTime;
        }
        
        return _myTime;                                 
    }
    
    obj.onKey = function(theKey, theState, theX, theY, theShiftFlag) {
        if (theState) {
            switch (theKey) {
            case 'R':
                print((_myEnabled ? "Disable" : "Enable") + " video recording.");
                _myEnabled = !_myEnabled;                
                break;
            }
        }            
    }
}

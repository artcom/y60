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
//   $RCSfile: HeartbeatThrober.js,v $
//   $Author: christian $
//   $Revision: 1.3 $
//   $Date: 2004/11/10 18:37:33 $
//
//=============================================================================

function HeartbeatThrober(theEnableFlag, theFrequency, theFilename) {
    var _myFrequency      = theFrequency; // in seconds
    var _myLastThrobTime  = -1;
    var _IsEnabled        = theEnableFlag;
    var _myFilename       = theFilename;
    var _myFirstThrob     = true;
    this.throb = function(theTime) {
        if (_myLastThrobTime == -1) {
            _myLastThrobTime = theTime;
            _myFirstThrob = true;
        }
        if (_IsEnabled) {
            var myDeltaTime = theTime - _myLastThrobTime;
            if (myDeltaTime > _myFrequency || _myFirstThrob) {
                _myFirstThrob = false;
                var mySecondsSince1970 = Date.parse(Date());
                var myHeartbeatString =
                    '<heartbeat secondsSince1970="' + mySecondsSince1970 +'" >\n' +
                    '</heartbeat>';
                var myHeartbeatDoc = new Node(myHeartbeatString);
                myHeartbeatDoc.saveFile(_myFilename);
                _myLastThrobTime = theTime;        
            }
        }
    }
    this.use = function(theFlag, theFrequency, theHeartbeatfile) {
        _IsEnabled       = theFlag;
        _myFrequency     = theFrequency;
        _myFilename      = theHeartbeatfile;
        _myLastThrobTime = -1;
    }
    
    this.enable = function(theFlag) {
        _IsEnabled = theFlag;
        _myFirstThrob = true;
    }

}

//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: StatusBar.js,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/04/20 16:56:33 $
//
//
//=============================================================================

function StatusBar(theGladeStatusBar) {
    var _myStatusBar      = theGladeStatusBar;
    var _myStartTime      = 0;
    var _myStatisticsFlag = false;

    var _myLabel = new Label();
    _myStatusBar.pack_end(_myLabel, false, false);

    this.set = function(theMessage) {
        _myStatusBar.pop();
        _myStatusBar.push(theMessage);
        _myStartTime = millisec();
    }

    this.enableStatistics = function(theFlag) {
        _myStatisticsFlag = theFlag;
        if (theFlag) {
            _myLabel.show();
        } else {
            _myLabel.hide();
        }
    }

    this.onFrame = function() {
        if (_myStatisticsFlag) {
            _myLabel.text = window.frameRate.toFixed(0) + " fps";
        }

        if (_myStartTime) {
            if ((millisec() - _myStartTime) > 3000) {
                _myStatusBar.pop();
                _myStartTime = 0;
            }
        }
    }
}

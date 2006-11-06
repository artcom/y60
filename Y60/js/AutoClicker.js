//=============================================================================
// Copyright (C) 2004-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
const DEFAULT_CLICK_INTERVAL = 0.5;

use("Y60JSSL.js");

function AutoClicker(theViewer, theInterval) {

    var Public = this;

    //////////////////////////////////////////////////////////////////////
    // Public
    //////////////////////////////////////////////////////////////////////
    Public.onFrame = function() {
        var myNow = millisec();
        var myTimeDelta = myNow - _myStartTime;
        if (myTimeDelta >= _myInterval * 1000) {
            _myStartTime = myNow;
            click();
        }
    }
    
    Public.Interval setter = function(theInterval) {
        _myInterval = theInterval;
    }
        
    Public.enabled setter = function(theFlag) {
        _myEnabledFlag = theFlag;
        if (_myEnabledFlag) {
            _myStartTime = millisec();
        }
    }

    //////////////////////////////////////////////////////////////////////
    // Private
    //////////////////////////////////////////////////////////////////////
    function click() {
        var myRandomX = randomInteger(0, _myViewer.getRenderWindow().width);
        var myRandomY = randomInteger(0, _myViewer.getRenderWindow().height);
        _myViewer.onMouseButton(LEFT_BUTTON, true, myRandomX, myRandomY);
    }
    
    var _myEnabledFlag = false;
    var _myStartTime   = null;
    var _myViewer      = theViewer;
    if (theInterval == undefined) {
        theInterval = DEFAULT_CLICK_INTERVAL;
    }
    var _myInterval = theInterval;
 }

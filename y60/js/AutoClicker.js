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
const DEFAULT_CLICK_INTERVAL = 0.5;

use("Y60JSSL.js");

function AutoClicker(theViewer, theInterval) {

    var Public = this;

    //////////////////////////////////////////////////////////////////////
    // Public
    //////////////////////////////////////////////////////////////////////
    Public.onFrame = function() {
        if (_myEnabledFlag) {
            var myNow = millisec();
            var myTimeDelta = myNow - _myStartTime;
            if (myTimeDelta >= _myInterval * 1000) {
                _myStartTime = myNow;
                click();
            }
        }
    }

    Public.Interval setter = function(theInterval) {
        _myInterval = theInterval;
    }

    Public.enabled getter = function() {
        return _myEnabledFlag;
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

//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: Timer.js,v $
//   $Author: christian $
//   $Revision: 1.9 $
//   $Date: 2004/02/23 09:05:07 $
//
//
//=============================================================================

var ENABLE_TIMING = true;

// This timer automatically times the lapsed time between two adds
// Sample usage:
//
// var _myTimer = new AutoTimer("foo");
// function onFrame() {
//      _myTimer.add("start");
//      foo1();
//      _myTimer.add("foo1");
//      foo2();
//      _myTimer.add("foo2");
// }

function AutoTimer(theName) {
    var _myName       = theName;
    var _myTimes      = [];
    var _myTimerNames = [];

    this.add = function(theName) {
        if (ENABLE_TIMING) {
            if (theName in _myTimerNames) {
                var myIndex = _myTimerNames[theName];
                _myTimes[myIndex] = millisec();
            } else {
                _myTimes.push(millisec());
                _myTimerNames[theName] = _myTimes.length - 1;
            }
        }
    }

    this.print = function() {
        if (ENABLE_TIMING) {
            print(_myName + " timing:");

            var myMaxNameLength = 0;
            for (var myTimerName in _myTimerNames) {
                if (myTimerName.length > myMaxNameLength) {
                    myMaxNameLength = myTimerName.length;
                }
            }
            for (var myTimerName in _myTimerNames) {
                var myIndex = _myTimerNames[myTimerName];
                if (myIndex > 0) {
                    var myPadding = "";
                    for (var i = 0; i < myMaxNameLength - myTimerName.length; ++i) {
                        myPadding += " ";
                    }

                    print('  ' + myTimerName + ':  ' + myPadding + Number(_myTimes[myIndex] - _myTimes[myIndex-1]).toFixed(3));
                }
            }
            var myPadding = "";
            for (var i = 0; i < myMaxNameLength - 5; ++i) {
                myPadding += " ";
            }
            print('  TOTAL:  ' + myPadding + Number(_myTimes[_myTimes.length - 1] - _myTimes[0]).toFixed(3));
        }
    }
}

function Timer(theName) {
    this.Constructor(this, theName);
}

Timer.prototype.Constructor = function(obj, theName) {
    var _myName       = theName;
    var _myTimes      = [];
    var _myElapsed    = [];
    var _myCounts     = [];
    var _myTimerNames = [];

    obj.add = function(theName, theCount) {
        if (theCount == undefined) {
            theCount = 1;
        }
		var myIndex = _myTimes.length;
        while (theName in _myTimerNames) {
            theName = theName + "_";
        }
		_myTimes.push(millisec());
        _myCounts.push(theCount);
        _myElapsed.push(0);
        _myTimerNames[theName] = myIndex;
        return myIndex;
    }

    obj.start = function(theIndex) {
		_myTimes[theIndex] = millisec();
    }

    obj.stop = function(theIndex) {
//print("theIndex="+theIndex);
//        dumpstack();
		_myElapsed[theIndex] = millisec() - _myTimes[theIndex];
    }
/*
    obj.print = function() {
        if (ENABLE_TIMING) {
            print(_myName + " timing:");
            for (var myTimerName in _myTimerNames) {
                var myIndex = _myTimerNames[myTimerName];
                if (myIndex >= 0) {
					var myTime = Number(_myElapsed[myIndex]);
					var myPerSecond = _myCounts[myIndex] * 1000 / myTime;
                    print('  ' + myTimerName + ': \t' + myTime.toFixed(3)+", "+ myPerSecond.toFixed(1) +" per sec.");
                }
            }
            print('  TOTAL: \t'+ Number(_myTimes[_myTimes.length - 1] - _myTimes[0]).toFixed(3));
        }
    }
*/
   this.print = function() {
        if (ENABLE_TIMING) {
            print(_myName + " timing:");

            var myMaxNameLength = 0;
            for (var myTimerName in _myTimerNames) {
                if (myTimerName.length > myMaxNameLength) {
                    myMaxNameLength = myTimerName.length;
                }
            }
            for (var myTimerName in _myTimerNames) {
                var myIndex = _myTimerNames[myTimerName];
                if (myIndex > 0) {
                    var myPadding = "";
                    for (var i = 0; i < myMaxNameLength - myTimerName.length; ++i) {
                        myPadding += " ";
                    }
					var myTime = Number(_myElapsed[myIndex]);
					var myPerSecond = _myCounts[myIndex] * 1000 / myTime;
                    print('  ' + myTimerName + ':  '  + myPadding + myTime.toFixed(3)+", "+ myPerSecond.toFixed(1) +" per sec.");
                }
            }
        }
    }
}

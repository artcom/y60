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
//
//   $RCSfile: Timer.js,v $
//   $Author: christian $
//   $Revision: 1.9 $
//   $Date: 2004/02/23 09:05:07 $
//
//
//=============================================================================

/*jslint nomen:false plusplus:false*/
/*globals millisec, print*/

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

    this.add = function (theName) {
        if (ENABLE_TIMING) {
            if (theName in _myTimerNames) {
                var myIndex = _myTimerNames[theName];
                _myTimes[myIndex] = millisec();
            } else {
                _myTimes.push(millisec());
                _myTimerNames[theName] = _myTimes.length - 1;
            }
        }
    };

    this.print = function () {
        if (ENABLE_TIMING) {
            var myTimerName, myPadding, i;
            print(_myName + " timing:");

            var myMaxNameLength = 0;
            for (myTimerName in _myTimerNames) {
                if (myTimerName.length > myMaxNameLength) {
                    myMaxNameLength = myTimerName.length;
                }
            }
            for (myTimerName in _myTimerNames) {
                var myIndex = _myTimerNames[myTimerName];
                if (myIndex > 0) {
                    myPadding = "";
                    for (i = 0; i < myMaxNameLength - myTimerName.length; ++i) {
                        myPadding += " ";
                    }

                    print('  ' + myTimerName + ':  ' + myPadding + Number(_myTimes[myIndex] - _myTimes[myIndex - 1]).toFixed(3));
                }
            }
            myPadding = "";
            for (i = 0; i < myMaxNameLength - 5; ++i) {
                myPadding += " ";
            }
            print('  TOTAL:  ' + myPadding + Number(_myTimes[_myTimes.length - 1] - _myTimes[0]).toFixed(3));
        }
    };
}

function Timer(theName) {
    this.Constructor(this, theName);
}

Timer.prototype.Constructor = function (obj, theName) {
    var _myName       = theName;
    var _myTimes      = [];
    var _myElapsed    = [];
    var _myCounts     = [];
    var _myTimerNames = [];

    obj.add = function (theName, theCount) {
        theCount = theCount || 1;
        var myIndex = _myTimes.length;
        while (theName in _myTimerNames) {
            theName = theName + "_";
        }
        _myTimes.push(millisec());
        _myCounts.push(theCount);
        _myElapsed.push(0);
        _myTimerNames[theName] = myIndex;
        return myIndex;
    };

    obj.start = function (theIndex) {
        _myTimes[theIndex] = millisec();
    };

    obj.stop = function (theIndex) {
        //print("theIndex="+theIndex);
        //dumpstack();
        _myElapsed[theIndex] = millisec() - _myTimes[theIndex];
    };

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
    this.print = function () {
        if (ENABLE_TIMING) {
            var myTimerName;
            print(_myName + " timing:");

            var myMaxNameLength = 0;
            for (myTimerName in _myTimerNames) {
                if (myTimerName.length > myMaxNameLength) {
                    myMaxNameLength = myTimerName.length;
                }
            }
            for (myTimerName in _myTimerNames) {
                var myIndex = _myTimerNames[myTimerName];
                if (myIndex !== undefined && myIndex >= 0 && myIndex < _myTimes.length) {
                    var myPadding = "";
                    for (var i = 0; i < myMaxNameLength - myTimerName.length; ++i) {
                        myPadding += " ";
                    }
                    var myTime = Number(_myElapsed[myIndex]);
                    var myPerSecond = _myCounts[myIndex] * 1000 / myTime;
                    print('  ' + myTimerName + ':  '  + myPadding + myTime.toFixed(3) + ", " + myPerSecond.toFixed(1) + " per sec.");
                }
            }
        }
    };
};

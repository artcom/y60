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


plug("OscReceiver");
use("spark/multitouch.js");

function RadarTouch(theStage) {
    this.Constructor(this, theStage);
}

RadarTouch.prototype.Constructor = function(Public, theStage) {

    var _myOscReceiver = null;
    var _myLastReceivedTime = 0;
    var _myTimeoutCallback = null;
    
    var _myLastPosition = [0,0];

    var _myCursors = [];
    
    var _myStage = theStage;
    var _myWorkspace = null;

    var _myLeftMargin = 0;
    var _myRightMargin = 0;
    var _myTopMargin = 0;
    var _myBottomMargin = 0;

    Public.Constructor = function() {
    }
    
    Public.start = function(thePort) {
        print("Starting OSC receiver on port: " + thePort);
        if (!_myOscReceiver) {
            _myOscReceiver = new OscReceiver(thePort);
        }
        _myOscReceiver.start();
    }
    
    Public.stop = function() {
        if (_myOscReceiver) {
            _myOscReceiver.stop();
        }
    }

    Public.flush = function() {
        _myOscReceiver && _myOscReceiver.flush();
    }
    
    Public.registerTimeoutCallback = function(theFunc) {
        _myTimeoutCallback = theFunc;
    }

    Public.topmargin setter = function(theMargin) {
        _myTopMargin = theMargin;
    }
    Public.topmargin getter = function() {
        return _myTopMargin;
    }
    Public.bottommargin setter = function(theMargin) {
        _myBottomMargin = theMargin;
    }
    Public.bottommargin getter = function() {
        return _myBottomMargin;
    }
    Public.leftmargin setter = function(theMargin) {
        _myLeftMargin = theMargin;
    }
    Public.leftmargin getter = function() {
        return _myLeftMargin;
    }
    Public.rightmargin setter = function(theMargin) {
        _myRightMargin = theMargin;
    }
    Public.rightmargin getter = function() {
        return _myRightMargin;
    }

    Public.onUpdateSettings = function(theNode) {
        for (var i = 0; i < theNode.childNodesLength(); i++) {
            var myChildNode = theNode.childNode(i);
            switch (myChildNode.nodeName) {
                case "LeftMargin":
                    _myLeftMargin = Number(myChildNode.childNode("#text").nodeValue);
                    break;
                case "RightMargin":
                    _myRightMargin = Number(myChildNode.childNode("#text").nodeValue);
                    break;
                case "TopMargin":
                    _myTopMargin = Number(myChildNode.childNode("#text").nodeValue);
                    break;
                case "BottomMargin":
                    _myBottomMargin = Number(myChildNode.childNode("#text").nodeValue);
                    break;
            }
        }
    }
    

    //////////////////////////////////////////////////////////////////////
    // Callbacks
    //////////////////////////////////////////////////////////////////////

    Public.onOscEvent = function(theNode) {

        var myAdrStr = theNode.type;
        var myArgs = [];
        var myEventName = "";

        _myLastReceivedTime = millisec();
                
        for (var i = 0; i < theNode.childNodesLength(); i++) {
            var myChild = theNode.childNode(i);
            if (myChild.nodeName == "float" || 
                myChild.nodeName == "int") {
                myArgs.push(Number(myChild.childNode("#text").nodeValue));
            } else {
                Logger.error("Unknown argument type: " + myChild.nodeName);
            }
        }

        if (myArgs.length == 0) {
            return;
        }
        
        // print("osc node: " + theNode);

        // map multitouch events to single touch events!

        switch (myAdrStr) {
            case "add":
                for (var i = 0; i < myArgs.length; i++) {
                    if (!(myArgs[i] in _myCursors)) {
                        var myCursor = new spark.Cursor(myArgs[i]);
                        myCursor.add_first = true;
                        _myCursors[myArgs[i]] = myCursor;
                        Logger.trace("Add event received: id=" + myCursor.id);
                    }
                }
                break;
            case "remove":
                for (var i = 0; i < myArgs.length; i++) {
                    if (myArgs[i] in _myCursors) {
                        var myCursor = _myCursors[myArgs[i]];
                        if (myCursor.add_first) {
                            // just to keep the balance right..
                            Public.dispatchCursorEvent( spark.CursorEvent.APPEAR, myCursor);
                        }
                        Public.dispatchCursorEvent( spark.CursorEvent.VANISH, myCursor );
                        delete _myCursors[myArgs[i]];
                    }
                }
                break;
            case "move":
                for (var i = 0; i < myArgs.length; i=i+3) {
                    Logger.trace( "got move event with id: " + myArgs[i] ); 
                    var myCoords = Public.planeToWindowCoords(myArgs[i+1], myArgs[i+2]); 
                    // look if we have a move without prior add: 
                    if (!(myArgs[i] in _myCursors)) {
                        var myCursor = new spark.Cursor(myArgs[i]);
                        myCursor.add_first = false;
                        myCursor.update(false, myCoords);
                        _myCursors[myArgs[i]] = myCursor;
                        Public.dispatchCursorEvent( spark.CursorEvent.APPEAR, myCursor );
                        break;
                    } else {
                        var myCursor = _myCursors[myArgs[i]];
                        myCursor.update(false, myCoords);
                        if (myCursor.add_first) {
                            Public.dispatchCursorEvent( spark.CursorEvent.APPEAR, myCursor );
                            myCursor.add_first = false;
                            break;
                        }
                        Logger.trace("Move event received: id=" + myCursor.id 
                                     + ", position=" + myCoords);
                        Public.dispatchCursorEvent( spark.CursorEvent.MOVE, myCursor );
                    }
                }
                break;
             case "alive":
                break;
             default:
                Logger.error("Strange osc message: " + theNode);
                break;
        }

    }
    
    Public.planeToWindowCoords = function(theX, theY) {
        var myWindowCoords = new Point2f(theX * (window.width - _myLeftMargin - _myRightMargin) + _myLeftMargin, 
                                         (1-theY) * (window.height - _myTopMargin - _myBottomMargin) + _myBottomMargin);
        return myWindowCoords;
    }
    
    Public.dispatchCursorEvent = function( theType, theCursor ) {
        var myCursorEvent = new spark.CursorEvent( theType, theCursor );
        var myPickedWidget = _myStage.pickWidget( theCursor.stageX, theCursor.stageY );
        if (!myPickedWidget) {
            myPickedWidget = _myStage;
        }
        myPickedWidget.dispatchEvent( myCursorEvent );
    }
    
}

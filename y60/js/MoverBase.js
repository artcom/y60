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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

if ((operatingSystem() == "LINUX" || operatingSystem() == "OSX") && expandEnvironment("${Y60_POWERMATE_SUPPORT}") == 1) {
    plug("PowermateInputExtension");
}

var LEFT_BUTTON   = 1;
var MIDDLE_BUTTON = 3;
var RIGHT_BUTTON  = 2;

var BUTTON_UP     = 0;
var BUTTON_DOWN   = 1;


function MoverBase(theViewport) {
    this.Constructor(this, theViewport);
};

MoverBase.prototype.Constructor = function(obj, theViewport) {

    const DOUBLE_CLICK_INTERVAL = 500;

    var _myViewport              = theViewport ? theViewport : window.canvas.childNode("viewport");
    var _myMoverObject           = null;
    var _myWorldSize             = null;

    var _myInitialPosition       = null;
    var _myInitialOrientation    = null;

    var _myLeftButtonFlag        = false;
    var _myMiddleButtonFlag      = false;
    var _myRightButtonFlag       = false;
    var _myDoubleLeftButtonFlag  = false;
    var _myDoubleClickInterval   = DOUBLE_CLICK_INTERVAL;


    //////////////////////////////////////////////////////////////////////
    //
    // public
    //
    //////////////////////////////////////////////////////////////////////

    obj.name = "Mover";

    obj.setMoverObject = function(theMoverObject) {
        if (theMoverObject == null) {
            return;
        }

        // store initial position for reset
        _myInitialPosition    = theMoverObject.position.value;
        _myInitialOrientation = new Quaternionf(theMoverObject.orientation);
        _myMoverObject = theMoverObject;

        obj.reset();
    }

    obj.setup = function() {
    }
    obj.stop = function() {}
    obj.setInitialPose = function() {
        if (_myMoverObject) {
            _myInitialPosition    = _myMoverObject.position.value;
            _myInitialOrientation = new Quaternionf(_myMoverObject.orientation);
        }
    }

    obj.getViewport = function() {
        return _myViewport;
    }

    obj.getViewportCamera = function() {
        return _myViewport.getElementById(_myViewport.camera);
    }

    obj.getWorld = function() {
        var myNodeInWorld = obj.getViewportCamera();
        while (myNodeInWorld.nodeName != "world") {
            myNodeInWorld = myNodeInWorld.parentNode;
        }
        return myNodeInWorld;
    }

    obj.getMoverObject = function() {
        return _myMoverObject;
    }

    obj.setDoubleClickActive = function(theFlag) {
        _myDoubleClickInterval = (theFlag) ? DOUBLE_CLICK_INTERVAL : 0;
    }
    obj.setWorldSize = function(theSize) {
        if (theSize) {
            _myWorldSize  = theSize;
        } else {
            _myWorldSize = 1.0;
        }
    }

    obj.getWorldSize = function() {
        return _myWorldSize;
    }

    obj.getLeftButtonFlag = function() {
        return _myLeftButtonFlag;
    }

    obj.getMiddleButtonFlag = function() {
        return _myMiddleButtonFlag;
    }

    obj.getRightButtonFlag = function() {
        return _myRightButtonFlag;
    }

    obj.getDoubleLeftButtonFlag = function() {
        return _myDoubleLeftButtonFlag;
    }
    
    obj.movements = {};

    obj.rotateWithObject = function(theVector) {
        var myWorldTranslation = new Vector4f(0,0,0,0);

        var myGlobalMatrix = _myMoverObject.globalmatrix;
        myWorldTranslation.add(product(myGlobalMatrix.getRow(0), theVector.x));
        myWorldTranslation.add(product(myGlobalMatrix.getRow(1), theVector.y));
        myWorldTranslation.add(product(myGlobalMatrix.getRow(2), theVector.z));
        return myWorldTranslation;
    }

    obj.update = function(theScreenTranslation, theWorldHeading) {
        var myGlobalMatrix = new Matrix4f(_myMoverObject.globalmatrix);
        var myWorldPosition  = myGlobalMatrix.getRow(3);

        // Move the camera
        var myWorldTranslation = obj.rotateWithObject(theScreenTranslation);
        myWorldPosition.add(myWorldTranslation);

        // Rotate about the world up vector
        myGlobalMatrix.rotateY(theWorldHeading);
        myGlobalMatrix.setRow(3, myWorldPosition);
        
        // Considering nested structure (camera within body node)
        var myParentMatrix = new Matrix4f(_myMoverObject.parentNode.globalmatrix);
        myParentMatrix.invert();
        myGlobalMatrix.postMultiply(myParentMatrix);

        // update MoverObject
        var myDecomposition = myGlobalMatrix.decompose();
        _myMoverObject.orientation = myDecomposition.orientation;
        _myMoverObject.position    = myGlobalMatrix.getTranslation();
    }

    //////////////////////////////////////////////////////////////////////

    obj.reset = function() {
        _myMoverObject.position    = _myInitialPosition;
        _myMoverObject.orientation = _myInitialOrientation;

        obj.setup();
    }

    obj.onKey = function(theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        if (theKeyState) {
            if (theShiftFlag) {
                theKey = theKey.toUpperCase();
            }
            switch (theKey) {
                case 'r':
                    obj.reset();
                    break;
                case "h":
                    printHelp();
                    break;
            }
        }
    }

    obj.onFrame = function(theTime) { // 'pure virtual'
    }

    var _myLastButtonTime = 0;
    obj.onMouseButton = function(theButton, theState, theX, theY) {
        switch (theButton) {
            case LEFT_BUTTON:
                _myLeftButtonFlag = (theState == BUTTON_DOWN);
                if (_myLeftButtonFlag) {
                    _myDoubleLeftButtonFlag = !_myDoubleLeftButtonFlag && ((millisec() - _myLastButtonTime) < _myDoubleClickInterval);
                    _myLastButtonTime = millisec();
                }
                break;
            case MIDDLE_BUTTON:
                _myMiddleButtonFlag = (theState == BUTTON_DOWN);
                break;
            case RIGHT_BUTTON:
                _myRightButtonFlag = (theState == BUTTON_DOWN);
                break;
            default:
                break;
        }
    }

    obj.onMouseMotion = function(theX, theY) { // 'pure virtual'
    }

    obj.onMouseWheel = function(theDeltaX, theDeltaY) {
    }

    obj.onAxis = function( theDevice, theAxis, theValue) {
    }

    obj.getNormalizedScreen = function(theMousePosX, theMousePosY) {
        return new Vector3f((theMousePosX * 2 -_myViewport.width)/_myViewport.width,
            -(theMousePosY * 2 -_myViewport.height)/_myViewport.height, 0);
    }

    function printHelp() {
        print("Mover keys:");
        print("    r    reset mover");
    }
}
